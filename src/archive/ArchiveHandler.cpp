// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "ArchiveHandler.hpp"
#include "../util/Configuration.hpp"
#include "../util/Logger.hpp"

#include <algorithm>
#include <format>
#include <ranges>

// miniz single-header archive API
#include "miniz.h"

namespace javatb {

bool ArchiveHandler::isArchiveExtension(std::string_view ext) {
    const auto& exts = Configuration::instance().getArrayProperty(ConfigurationProperty::ZIP_EXTENSIONS);
    return std::ranges::any_of(exts, [&](const std::string& e) { return e == ext; });
}

std::expected<std::vector<ArchiveHandler::ArchiveEntry>, std::string>
ArchiveHandler::readArchive(std::span<const std::byte> data) {
    mz_zip_archive zip{};
    mz_zip_zero_struct(&zip);

    if (!mz_zip_reader_init_mem(&zip,
                                data.data(),
                                data.size_bytes(),
                                0)) {
        return std::unexpected(std::string("miniz: failed to open archive in memory"));
    }

    std::vector<ArchiveEntry> entries;
    mz_uint numFiles = mz_zip_reader_get_num_files(&zip);
    entries.reserve(numFiles);

    for (mz_uint i = 0; i < numFiles; ++i) {
        mz_zip_archive_file_stat stat{};
        if (!mz_zip_reader_file_stat(&zip, i, &stat)) {
            LOG_WARN(std::format("miniz: could not stat file index {}", i));
            continue;
        }

        ArchiveEntry entry;
        entry.name           = stat.m_filename;
        entry.size           = static_cast<long long>(stat.m_uncomp_size);
        entry.compressedSize = static_cast<long long>(stat.m_comp_size);

        // DOS time → milliseconds since epoch
        if (stat.m_time != 0) {
            // m_time is std::time_t (seconds since epoch) for miniz >= 3.x
            entry.lastModifiedMs = static_cast<long long>(stat.m_time) * 1000LL;
        }

        // Extract uncompressed data
        if (!stat.m_is_directory) {
            entry.data.resize(static_cast<std::size_t>(stat.m_uncomp_size));
            if (!mz_zip_reader_extract_to_mem(&zip, i,
                                              entry.data.data(),
                                              entry.data.size(),
                                              0)) {
                LOG_WARN(std::format("miniz: failed to extract entry '{}'", entry.name));
                entry.data.clear();
            }
        }

        entries.push_back(std::move(entry));
    }

    mz_zip_reader_end(&zip);
    return entries;
}

} // namespace javatb
