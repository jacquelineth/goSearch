// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.util.FileUtils
#pragma once

#include <expected>
#include <filesystem>
#include <string>
#include <unordered_set>

// Forward declaration to avoid pulling in search headers here
namespace javatb { class SearchElement; }

namespace javatb {

namespace fs = std::filesystem;

struct FileUtils {
    FileUtils() = delete;

    static constexpr std::size_t BUFFER_SIZE = 32 * 1024;

    // ── File I/O ────────────────────────────────────────────────────────────
    [[nodiscard]] static std::expected<std::string, std::string>
        readTextFile(const fs::path& path);

    static std::expected<void, std::string>
        writeTextFile(const fs::path& path, std::string_view content);

    // ── Path helpers ────────────────────────────────────────────────────────
    /// Returns the lowercase extension without the leading dot, or "" if none.
    [[nodiscard]] static std::string getFileExtension(std::string_view filePath);
    [[nodiscard]] static std::string getFileExtension(const fs::path& file);

    /// Returns the filename without any directory component.
    [[nodiscard]] static std::string getFileShortName(std::string_view filePath);

    // ── Archive helpers ─────────────────────────────────────────────────────
    /// Returns the set of extensions treated as ZIP-compatible archives.
    [[nodiscard]] static const std::unordered_set<std::string>& zipExtensions();

    // ── Desktop actions ─────────────────────────────────────────────────────
    /// Open with the OS-associated default application.
    static void openSearchElement(const SearchElement& elt);

    /// Open with the OS-associated editor.
    static void editSearchElement(const SearchElement& elt);

    /// Reveal the element's containing folder in the OS file manager.
    static void exploreSearchElement(const SearchElement& elt);

    /// Recursively delete path (file or directory).
    static void deleteRecursive(const fs::path& root);
};

} // namespace javatb
