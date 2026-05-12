// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.search.SearchElement
#pragma once

#include <chrono>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace javatb {

/// Descriptor for a file, folder, or archive entry.
/// Objects are immutable after construction.
class SearchElement {
public:
    // ── Type flag constants (bitmask) ────────────────────────────────────────
    static constexpr int FILE    = 1;
    static constexpr int FOLDER  = 2;
    static constexpr int ARCHIVE = 4;
    static constexpr int ENTRY   = 8;

    using TimePoint = std::chrono::system_clock::time_point;
    using PathList  = std::vector<std::shared_ptr<SearchElement>>;

    // ── Constructors ─────────────────────────────────────────────────────────

    /// Construct from a raw name, type flags, and parent path chain.
    SearchElement(std::string name, int type, PathList path);

    /// Construct from a filesystem directory_entry (populates metadata).
    SearchElement(const std::filesystem::directory_entry& entry, int type, PathList path);

    /// Construct from a miniz archive entry descriptor.
    struct ArchiveEntryInfo {
        std::string name;
        long long   size             = -1LL;
        long long   compressedSize   = -1LL;
        long long   lastModifiedMs   = -1LL; ///< ms since epoch, -1 if unknown
    };
    SearchElement(const ArchiveEntryInfo& info, int type, PathList path);

    // ── Type queries ─────────────────────────────────────────────────────────
    [[nodiscard]] bool isFile()         const noexcept { return (type_ & FILE)    != 0; }
    [[nodiscard]] bool isFolder()       const noexcept { return (type_ & FOLDER)  != 0; }
    [[nodiscard]] bool isArchive()      const noexcept { return (type_ & ARCHIVE) != 0; }
    [[nodiscard]] bool isArchiveEntry() const noexcept { return (type_ & ENTRY)   != 0; }
    [[nodiscard]] int  type()           const noexcept { return type_; }

    // ── Accessors ────────────────────────────────────────────────────────────
    [[nodiscard]] const std::string&  getName()           const noexcept { return name_; }
    [[nodiscard]] long long           getSize()           const noexcept { return size_; }
    [[nodiscard]] long long           getCompressedSize() const noexcept { return compressedSize_; }
    [[nodiscard]] const TimePoint&    getLastModified()   const noexcept { return lastModified_; }
    [[nodiscard]] bool                hasLastModified()   const noexcept { return hasLastModified_; }
    [[nodiscard]] const std::string&  getAttributes()     const noexcept { return attributes_; }
    [[nodiscard]] const PathList&     getPath()           const noexcept { return path_; }

    /// Reconstruct the full display path (ancestor chain + name).
    [[nodiscard]] std::string toString() const;

private:
    int         type_           = 0;
    std::string name_;
    PathList    path_;
    long long   size_           = -1LL;
    long long   compressedSize_ = -1LL;
    TimePoint   lastModified_;
    bool        hasLastModified_ = false;
    std::string attributes_;
};

} // namespace javatb
