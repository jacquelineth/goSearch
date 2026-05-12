// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "FileUtils.hpp"
#include "Configuration.hpp"
#include "Logger.hpp"
#include "../search/SearchElement.hpp"
#include "../archive/ArchiveHandler.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_set>

#include <QDesktopServices>
#include <QUrl>
#include <QString>
#include <QProcess>
#include <QTemporaryFile>
#include <QDir>
#include <QStandardPaths>

namespace javatb {

// ── File I/O ─────────────────────────────────────────────────────────────────

std::expected<std::string, std::string>
FileUtils::readTextFile(const fs::path& path) {
    std::ifstream ifs(path, std::ios::in);
    if (!ifs.is_open())
        return std::unexpected(std::format("Cannot open file: {}", path.string()));
    std::ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
}

std::expected<void, std::string>
FileUtils::writeTextFile(const fs::path& path, std::string_view content) {
    std::ofstream ofs(path, std::ios::out | std::ios::trunc);
    if (!ofs.is_open())
        return std::unexpected(std::format("Cannot write file: {}", path.string()));
    ofs << content;
    return {};
}

// ── Path helpers ─────────────────────────────────────────────────────────────

std::string FileUtils::getFileExtension(std::string_view filePath) {
    auto idx = filePath.rfind('.');
    if (idx == std::string_view::npos) return {};
    auto ext = filePath.substr(idx + 1);
    // Guard against e.g. ".hidden" — make sure no path separators after the dot
    if (ext.contains('/') || ext.contains('\\')) return {};
    std::string result(ext);
    std::ranges::transform(result, result.begin(), [](unsigned char c){ return std::tolower(c); });
    return result;
}

std::string FileUtils::getFileExtension(const fs::path& file) {
    auto ext = file.extension().string();
    if (!ext.empty() && ext.front() == '.') ext = ext.substr(1);
    std::ranges::transform(ext, ext.begin(), [](unsigned char c){ return std::tolower(c); });
    return ext;
}

std::string FileUtils::getFileShortName(std::string_view filePath) {
    auto idx1 = filePath.rfind('/');
    auto idx2 = filePath.rfind('\\');
    std::size_t idx = std::string_view::npos;
    if (idx1 != std::string_view::npos && idx2 != std::string_view::npos)
        idx = std::max(idx1, idx2);
    else if (idx1 != std::string_view::npos) idx = idx1;
    else if (idx2 != std::string_view::npos) idx = idx2;

    if (idx == std::string_view::npos) return std::string(filePath);
    return std::string(filePath.substr(idx + 1));
}

// ── Archive extensions ────────────────────────────────────────────────────────

const std::unordered_set<std::string>& FileUtils::zipExtensions() {
    static std::unordered_set<std::string> cache = []() {
        auto tokens = Configuration::instance().getArrayProperty(ConfigurationProperty::ZIP_EXTENSIONS);
        std::unordered_set<std::string> s;
        for (auto& t : tokens) {
            std::ranges::transform(t, t.begin(), [](unsigned char c){ return std::tolower(c); });
            s.insert(std::move(t));
        }
        return s;
    }();
    return cache;
}

// ── Desktop helpers ───────────────────────────────────────────────────────────

namespace {
/// Extract an archive entry to a temp file and return the path.
/// Returns an empty path on failure.
fs::path extractEntryToTemp(const SearchElement& elt);
} // anonymous namespace

void FileUtils::openSearchElement(const SearchElement& elt) {
    if (elt.isFile()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(elt.toString())));
    } else if (elt.isArchiveEntry()) {
        fs::path tmp = extractEntryToTemp(elt);
        if (!tmp.empty())
            QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(tmp.string())));
    }
}

void FileUtils::editSearchElement(const SearchElement& elt) {
    // Qt does not have a separate "edit" action; open is used for editing too.
    openSearchElement(elt);
}

void FileUtils::exploreSearchElement(const SearchElement& elt) {
    fs::path target;
    if (elt.isFile()) {
        target = fs::path(elt.toString()).parent_path();
    } else if (elt.isArchiveEntry() && !elt.getPath().empty()) {
        target = fs::path(elt.getPath().front()->getName()).parent_path();
    } else {
        return;
    }

    QString dirStr = QString::fromStdString(target.string());
#if defined(_WIN32)
    QProcess::startDetached("explorer", {dirStr});
#elif defined(__APPLE__)
    QProcess::startDetached("open", {dirStr});
#else
    // Try common Linux file managers
    if (!QProcess::startDetached("xdg-open", {dirStr}))
        QProcess::startDetached("nautilus", {"--no-desktop", dirStr});
#endif
}

void FileUtils::deleteRecursive(const fs::path& root) {
    std::error_code ec;
    fs::remove_all(root, ec);
    if (ec)
        LOG_ERR(std::format("deleteRecursive failed for {}: {}", root.string(), ec.message()));
}

// ── Private: extract archive entry ───────────────────────────────────────────

namespace {

fs::path extractEntryToTemp(const SearchElement& elt) {
    using namespace javatb;
    const auto& parents = elt.getPath();
    if (parents.empty()) return {};

    // Read the root archive from disk
    fs::path rootPath(parents.front()->getName());
    std::ifstream ifs(rootPath, std::ios::binary);
    if (!ifs) return {};
    std::vector<std::byte> data((std::istreambuf_iterator<char>(ifs)),
                                 std::istreambuf_iterator<char>());

    // Traverse nested archives
    for (std::size_t i = 1; i < parents.size(); ++i) {
        auto result = ArchiveHandler::readArchive(data);
        if (!result) return {};
        bool found = false;
        for (auto& entry : *result) {
            if (entry.name == parents[i]->getName()) {
                data = std::move(entry.data);
                found = true;
                break;
            }
        }
        if (!found) return {};
    }

    // Find the leaf entry
    auto result = ArchiveHandler::readArchive(data);
    if (!result) return {};
    for (auto& entry : *result) {
        if (entry.name == elt.getName()) {
            // Write to temp file
            auto shortName = QString::fromStdString(FileUtils::getFileShortName(elt.getName()));
            auto tmpDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
            QString tmpPath = tmpDir + "/" + shortName;
            QFile tmpFile(tmpPath);
            if (tmpFile.open(QIODevice::WriteOnly)) {
                tmpFile.write(reinterpret_cast<const char*>(entry.data.data()),
                              static_cast<qint64>(entry.data.size()));
                tmpFile.close();
                return fs::path(tmpPath.toStdString());
            }
            break;
        }
    }
    return {};
}
} // anonymous namespace

} // namespace javatb
