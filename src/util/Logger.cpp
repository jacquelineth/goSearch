// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "Logger.hpp"

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string_view>

namespace javatb {

struct Logger::Impl {
    std::ofstream fileOut;
    LogLevel minLevel = LogLevel::INFO;
    std::mutex mutex;
};

Logger::Logger() : impl_(std::make_unique<Impl>()) {
    impl_->fileOut.open("find4j.log", std::ios::app);
}

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

void Logger::setLevel(LogLevel minLevel) {
    std::lock_guard lock(impl_->mutex);
    impl_->minLevel = minLevel;
}

bool Logger::isDebugEnabled() const noexcept {
    return impl_->minLevel <= LogLevel::DEBUG;
}

namespace {
constexpr std::string_view levelName(LogLevel l) noexcept {
    switch (l) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::SEVERE:  return "SEVERE";
    }
    return "UNKNOWN";
}
} // anonymous namespace

void Logger::log(LogLevel level, std::string_view message, const std::source_location& loc) {
    std::lock_guard lock(impl_->mutex);
    if (level < impl_->minLevel) return;

    // Format timestamp as yyyy-MM-dd HH:mm:ss.SSS
    auto now = std::chrono::system_clock::now();
    auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    auto tt  = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf{};
#if defined(_WIN32)
    localtime_s(&tm_buf, &tt);
#else
    localtime_r(&tt, &tm_buf);
#endif
    std::string timeStr = std::format("{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}.{:03d}",
        tm_buf.tm_year + 1900, tm_buf.tm_mon + 1, tm_buf.tm_mday,
        tm_buf.tm_hour, tm_buf.tm_min, tm_buf.tm_sec,
        static_cast<int>(ms.count()));

    // Shorten file name (strip directory)
    std::string_view file = std::filesystem::path(loc.file_name()).filename().string();
    std::string line_str = std::format("[{}][{}:{}()] {}",
        levelName(level), file, loc.line(), message);

    std::string entry = std::format("{} {}\n", timeStr, line_str);

    if (impl_->fileOut.is_open())
        impl_->fileOut << entry << std::flush;

    // Console: always print WARNING and SEVERE
    if (level >= LogLevel::WARNING)
        std::cerr << entry;
    else if (level >= impl_->minLevel)
        std::cout << entry;
}

} // namespace javatb
