// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "SearchElement.hpp"

#include <filesystem>
#include <numeric>
#include <ranges>
#include <string>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

namespace javatb {

namespace fs = std::filesystem;

// ── Constructor: raw name + type + path ──────────────────────────────────────

SearchElement::SearchElement(std::string name, int type, PathList path)
    : type_(type)
    , name_(std::move(name))
    , path_(std::move(path))
{}

// ── Constructor: from filesystem::directory_entry ────────────────────────────

SearchElement::SearchElement(const fs::directory_entry& entry, int type, PathList path)
    : type_(type)
    , name_(entry.path().string())
    , path_(std::move(path))
{
    if (entry.is_regular_file() || entry.is_directory()) {
        std::error_code ec;
        auto sz = entry.file_size(ec);
        if (!ec) size_ = static_cast<long long>(sz);

        auto lwt = entry.last_write_time(ec);
        if (!ec) {
            // Convert file_time_type → system_clock::time_point
            auto sysTime = std::chrono::file_clock::to_sys(lwt);
            lastModified_    = std::chrono::time_point_cast<std::chrono::system_clock::duration>(sysTime);
            hasLastModified_ = true;
        }

        // Build "RWXH" attribute string
        auto p = entry.path();
        std::string attr;
        // Readable: try opening for read
        {
            fs::perms perms = entry.status(ec).permissions();
            bool readable  = (perms & fs::perms::owner_read)  != fs::perms::none;
            bool writable  = (perms & fs::perms::owner_write) != fs::perms::none;
            bool executable= (perms & fs::perms::owner_exec)  != fs::perms::none;
            attr += readable   ? 'R' : '.';
            attr += writable   ? 'W' : '.';
            attr += executable ? 'X' : '.';
        }
        // Hidden: starts with '.' on Unix; FILE_ATTRIBUTE_HIDDEN on Windows (best effort)
        {
            auto fname = p.filename().string();
#if defined(_WIN32)
            DWORD attrs = GetFileAttributesW(p.wstring().c_str());
            bool hidden = (attrs != INVALID_FILE_ATTRIBUTES) && (attrs & FILE_ATTRIBUTE_HIDDEN);
#else
            bool hidden = !fname.empty() && fname.front() == '.';
#endif
            attr += hidden ? 'H' : '.';
        }
        attributes_ = std::move(attr);
    }
}

// ── Constructor: from archive entry info ─────────────────────────────────────

SearchElement::SearchElement(const ArchiveEntryInfo& info, int type, PathList path)
    : type_(type)
    , name_(info.name)
    , path_(std::move(path))
    , size_(info.size)
    , compressedSize_(info.compressedSize)
    , attributes_("R...")
{
    if (info.lastModifiedMs > 0) {
        lastModified_ = std::chrono::system_clock::time_point(
            std::chrono::milliseconds(info.lastModifiedMs));
        hasLastModified_ = true;
    }
}

// ── toString ─────────────────────────────────────────────────────────────────

std::string SearchElement::toString() const {
    if (path_.empty()) return name_;
    // Build: ancestor0/name0 / ancestor1/name1 / ... / this->name_
    return std::ranges::fold_left(path_, std::string{},
        [](std::string acc, const std::shared_ptr<SearchElement>& elt) {
            return acc + elt->getName() +
                   std::string(1, static_cast<char>(std::filesystem::path::preferred_separator));
        }) + name_;
}

} // namespace javatb
