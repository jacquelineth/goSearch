// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// ZIP archive handler wrapping miniz
#pragma once

#include <cstddef>
#include <expected>
#include <span>
#include <string>
#include <vector>

namespace javatb {

struct ArchiveHandler {
    ArchiveHandler() = delete;

    struct ArchiveEntry {
        std::string            name;
        long long              size           = -1LL;
        long long              compressedSize = -1LL;
        long long              lastModifiedMs = -1LL;
        std::vector<std::byte> data;          ///< Decompressed entry contents
    };

    /// Check if the given (lowercased) extension is a supported archive format.
    [[nodiscard]] static bool isArchiveExtension(std::string_view ext);

    /// Read all entries from an in-memory ZIP archive.
    /// Returns a vector of entries, or an error string on failure.
    [[nodiscard]] static std::expected<std::vector<ArchiveEntry>, std::string>
        readArchive(std::span<const std::byte> data);
};

} // namespace javatb
