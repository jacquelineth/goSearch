// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "Configuration.hpp"
#include "StringUtils.hpp"

#include <filesystem>
#include <fstream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

namespace javatb {

namespace fs = std::filesystem;

struct Configuration::Impl {
    std::map<std::string, std::string> props;
    mutable std::mutex mutex;
};

namespace {
/// Trim leading and trailing whitespace from a string_view.
std::string trim(std::string_view sv) {
    const std::string_view WS = " \t\r\n";
    auto start = sv.find_first_not_of(WS);
    if (start == std::string_view::npos) return {};
    auto end = sv.find_last_not_of(WS);
    return std::string(sv.substr(start, end - start + 1));
}

/// Locate the config file: next to the executable, then in CWD, then on classpath.
fs::path findConfigFile() {
    // 1. Alongside the executable (resolved via argv[0] or proc maps)
    //    Fallback: use CWD.
    const auto candidates = {
        fs::path("config/config.properties"),
        fs::path("../config/config.properties"),
    };
    for (auto& p : candidates) {
        if (fs::exists(p)) return p;
    }
    return "config/config.properties"; // will fail gracefully on load
}
} // anonymous namespace

Configuration::Configuration() : impl_(std::make_unique<Impl>()) {
    load();
}

void Configuration::load() {
    std::lock_guard lock(impl_->mutex);
    impl_->props.clear();

    auto path = findConfigFile();
    std::ifstream file(path);
    if (!file.is_open()) return; // silently use defaults

    std::string line;
    while (std::getline(file, line)) {
        auto trimmed = trim(line);
        // Skip comments and blank lines
        if (trimmed.empty() || trimmed.starts_with('#') || trimmed.starts_with('!'))
            continue;
        // Split on first '='
        auto eq = trimmed.find('=');
        if (eq == std::string::npos) continue;
        auto key = trim(trimmed.substr(0, eq));
        auto val = trim(trimmed.substr(eq + 1));
        impl_->props[key] = val;
    }
}

Configuration& Configuration::instance() {
    static Configuration inst;
    return inst;
}

void Configuration::reset() {
    load();
}

std::string Configuration::getString(ConfigurationProperty p) const {
    std::lock_guard lock(impl_->mutex);
    auto key = std::string(propertyKey(p));
    auto it = impl_->props.find(key);
    if (it != impl_->props.end() && !it->second.empty())
        return it->second;
    return std::string(propertyDefault(p));
}

std::vector<std::string> Configuration::getArrayProperty(ConfigurationProperty p) const {
    std::string val = getString(p);
    std::vector<std::string> result;
    std::istringstream iss(val);
    std::string token;
    while (iss >> token) result.push_back(std::move(token));
    return result;
}

int Configuration::getInt(ConfigurationProperty p, int fallback) const {
    return StringUtils::toInt(getString(p), fallback);
}

} // namespace javatb
