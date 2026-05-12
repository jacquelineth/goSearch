// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "SearchEngine.hpp"
#include "../archive/ArchiveHandler.hpp"
#include "../util/FileUtils.hpp"
#include "../util/Logger.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <regex>
#include <sstream>
#include <span>

namespace javatb {

namespace fs = std::filesystem;

// ── Constructor ───────────────────────────────────────────────────────────────

SearchEngine::SearchEngine(std::vector<std::string> args) {
    rootPath_ = args.empty() ? "." : args[0];

    std::string pattern = (args.size() > 1 && !args[1].empty()) ? args[1] : "*";
    fileRegex_ = wildcardToRegex(pattern);

    if (args.size() > 2 && !args[2].empty()) {
        hasContentPattern_ = true;
        contentRegex_ = std::regex(args[2],
            std::regex_constants::icase | std::regex_constants::ECMAScript);
    }
}

// ── Listener management ───────────────────────────────────────────────────────

void SearchEngine::addSearchEngineListener(SearchEngineListener* l) {
    listeners_.push_back(l);
}

void SearchEngine::removeSearchEngineListener(SearchEngineListener* l) {
    std::erase(listeners_, l);
}

// ── Public: search() ─────────────────────────────────────────────────────────

void SearchEngine::search() {
    auto root = std::make_shared<SearchElement>(rootPath_, 0, SearchElement::PathList{});
    fireEvent(SearchEngineEvent::Type::START, root);
    try {
        // Support colon- or semicolon-separated root paths
        char sep =
#if defined(_WIN32)
            ';';
#else
            ':';
#endif
        std::string remaining = rootPath_;
        std::string part;
        while (!remaining.empty()) {
            auto pos = remaining.find(sep);
            if (pos == std::string::npos) {
                part = remaining;
                remaining.clear();
            } else {
                part = remaining.substr(0, pos);
                remaining = remaining.substr(pos + 1);
            }
            if (!part.empty())
                searchPath(fs::path(part));
        }
    } catch (const std::exception& e) {
        LOG_ERR(std::format("SearchEngine::search exception: {}", e.what()));
    }
    fireEvent(SearchEngineEvent::Type::END, root);
}

// ── Internal: directory traversal ────────────────────────────────────────────

void SearchEngine::searchPath(const fs::path& root) {
    if (isStopped()) return;

    std::error_code ec;
    if (!fs::exists(root, ec)) return;

    auto entry = fs::directory_entry(root, ec);
    if (ec) return;

    std::string name    = root.filename().string();
    std::string nameLow = name;
    std::ranges::transform(nameLow, nameLow.begin(),
        [](unsigned char c){ return std::tolower(c); });

    bool nameMatches = std::regex_match(nameLow, fileRegex_);

    if (entry.is_directory(ec)) {
        // Directories never match content search
        auto elt = std::make_shared<SearchElement>(root.string(), SearchElement::FOLDER,
                                                   getCurrentPath());
        fireEvent(SearchEngineEvent::Type::EXPLORING, elt);
        std::error_code itec;
        for (auto& child : fs::directory_iterator(root, itec)) {
            if (isStopped()) return;
            searchPath(child.path());
        }
        return;
    }

    // Regular file
    auto ext = FileUtils::getFileExtension(name);
    const auto& zipExts = FileUtils::zipExtensions();

    if (zipExts.contains(ext)) {
        // Archive file
        auto elt = std::make_shared<SearchElement>(entry,
            SearchElement::ARCHIVE | SearchElement::FILE, getCurrentPath());
        fireEvent(SearchEngineEvent::Type::EXPLORING, elt);
        pushPath(elt);
        try {
            std::ifstream ifs(root, std::ios::binary);
            if (ifs) {
                std::vector<std::byte> data(
                    (std::istreambuf_iterator<char>(ifs)),
                     std::istreambuf_iterator<char>());
                searchArchive(data, getCurrentPath());
            }
        } catch (const std::exception& e) {
            LOG_WARN(std::format("Error reading archive {}: {}", root.string(), e.what()));
        }
        popPath();
    } else {
        // Plain file
        auto elt = std::make_shared<SearchElement>(entry, SearchElement::FILE,
                                                   getCurrentPath());
        bool match = nameMatches;

        if (match && hasContentPattern_) {
            // Check available heap memory (rough heuristic): read only if file < 32 MB
            long long sz = elt->getSize();
            if (sz > 0 && sz < 32LL * 1024 * 1024) {
                auto content = FileUtils::readTextFile(root);
                if (content) {
                    match = std::regex_search(*content, contentRegex_);
                } else {
                    match = false;
                }
            } else {
                match = false;
            }
        }

        if (match)
            fireEvent(SearchEngineEvent::Type::MATCH, elt);
    }
}

// ── Internal: archive traversal ──────────────────────────────────────────────

void SearchEngine::searchArchive(std::span<const std::byte> data,
    const std::vector<std::shared_ptr<SearchElement>>& cpath) {
    if (isStopped()) return;

    auto result = ArchiveHandler::readArchive(data);
    if (!result) {
        LOG_WARN(std::format("ArchiveHandler failed: {}", result.error()));
        return;
    }

    for (auto& ae : *result) {
        if (isStopped()) return;

        std::string shortName = FileUtils::getFileShortName(ae.name);
        std::string shortLow  = shortName;
        std::ranges::transform(shortLow, shortLow.begin(),
            [](unsigned char c){ return std::tolower(c); });

        bool nameMatches = std::regex_match(shortLow, fileRegex_);
        auto ext = FileUtils::getFileExtension(ae.name);
        const auto& zipExts = FileUtils::zipExtensions();

        SearchElement::ArchiveEntryInfo info{ae.name, ae.size, ae.compressedSize, ae.lastModifiedMs};

        if (zipExts.contains(ext)) {
            // Nested archive
            auto elt = std::make_shared<SearchElement>(info,
                SearchElement::ARCHIVE | SearchElement::ENTRY, cpath);
            fireEvent(SearchEngineEvent::Type::EXPLORING, elt);
            pushPath(elt);
            auto nested = getCurrentPath();
            searchArchive(std::as_bytes(std::span(ae.data)), nested);
            popPath();
        } else {
            // Plain entry
            if (ae.data.empty() && ae.size > 0) continue; // extraction failed

            auto elt = std::make_shared<SearchElement>(info,
                SearchElement::ENTRY, cpath);
            bool match = nameMatches;

            if (match && hasContentPattern_ && !ae.data.empty()) {
                std::string_view content(
                    reinterpret_cast<const char*>(ae.data.data()), ae.data.size());
                match = std::regex_search(std::string(content), contentRegex_);
            }

            if (match)
                fireEvent(SearchEngineEvent::Type::MATCH, elt);
        }
    }
}

// ── Helpers ───────────────────────────────────────────────────────────────────

std::regex SearchEngine::wildcardToRegex(const std::string& pattern) {
    std::string re;
    re.reserve(pattern.size() * 2);
    for (char c : pattern) {
        switch (c) {
            case '*': re += ".*"; break;
            case '?': re += ".?"; break;
            case '.': re += "\\."; break;
            default:
                re += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                break;
        }
    }
    return std::regex(re, std::regex_constants::icase | std::regex_constants::ECMAScript);
}

void SearchEngine::fireEvent(SearchEngineEvent::Type type,
                             std::shared_ptr<SearchElement> element) {
    SearchEngineEvent ev(type, std::move(element));
    for (auto* l : listeners_) l->newMessage(ev);
}

void SearchEngine::pushPath(std::shared_ptr<SearchElement> elt) {
    std::lock_guard lock(pathMutex_);
    currentPath_.push_back(std::move(elt));
}

void SearchEngine::popPath() {
    std::lock_guard lock(pathMutex_);
    if (!currentPath_.empty()) currentPath_.pop_back();
}

std::vector<std::shared_ptr<SearchElement>> SearchEngine::getCurrentPath() const {
    std::lock_guard lock(pathMutex_);
    return {currentPath_.begin(), currentPath_.end()};
}

} // namespace javatb
