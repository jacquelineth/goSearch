// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.util.NiceLogFormatter + logging setup
#pragma once

#include <string>
#include <string_view>
#include <source_location>

namespace javatb {

enum class LogLevel { DEBUG, INFO, WARNING, SEVERE };

class Logger {
public:
    static Logger& instance();

    void log(LogLevel level,
             std::string_view message,
             const std::source_location& loc = std::source_location::current());

    void setLevel(LogLevel minLevel);
    [[nodiscard]] bool isDebugEnabled() const noexcept;

private:
    Logger();
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace javatb

// Convenience macros that capture source location automatically.
#define LOG_DEBUG(msg) \
    ::javatb::Logger::instance().log(::javatb::LogLevel::DEBUG, (msg), std::source_location::current())
#define LOG_INFO(msg) \
    ::javatb::Logger::instance().log(::javatb::LogLevel::INFO, (msg), std::source_location::current())
#define LOG_WARN(msg) \
    ::javatb::Logger::instance().log(::javatb::LogLevel::WARNING, (msg), std::source_location::current())
#define LOG_ERR(msg) \
    ::javatb::Logger::instance().log(::javatb::LogLevel::SEVERE, (msg), std::source_location::current())
