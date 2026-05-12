// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.util.Configuration
#pragma once

#include <string>
#include <vector>
#include <expected>
#include "ConfigurationProperty.hpp"

namespace javatb {

/// Singleton that loads and provides access to config/config.properties.
/// Thread-safe after the first call to instance().
class Configuration {
public:
    /// Get the singleton. Loads config on first call.
    static Configuration& instance();

    /// Reset (reload) configuration from disk.
    void reset();

    /// Get a raw string value, or the property default if the key is absent.
    [[nodiscard]] std::string getString(ConfigurationProperty p) const;

    /// Get a space-separated property as a vector of tokens.
    [[nodiscard]] std::vector<std::string> getArrayProperty(ConfigurationProperty p) const;

    /// Get an integer property; falls back to default string then to fallback param.
    [[nodiscard]] int getInt(ConfigurationProperty p, int fallback = 0) const;

private:
    Configuration();
    void load();

    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace javatb
