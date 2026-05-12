// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.util.ConfigurationProperty
#pragma once

#include <string_view>

namespace javatb {

enum class ConfigurationProperty {
    ZIP_EXTENSIONS,
    HISTORY_SIZE,
    RESULT_RENDERER,
};

/// Returns the property key name (as used in config.properties)
constexpr std::string_view propertyKey(ConfigurationProperty p) noexcept {
    switch (p) {
        case ConfigurationProperty::ZIP_EXTENSIONS:  return "org.javatb.search.zip.extensions";
        case ConfigurationProperty::HISTORY_SIZE:    return "org.javatb.history.size";
        case ConfigurationProperty::RESULT_RENDERER: return "org.javatb.renderer.class";
    }
    return "";
}

/// Returns the built-in default value string
constexpr std::string_view propertyDefault(ConfigurationProperty p) noexcept {
    switch (p) {
        case ConfigurationProperty::ZIP_EXTENSIONS:
            return "zip jar war ear rar pak ods odp odg odt";
        case ConfigurationProperty::HISTORY_SIZE:
            return "10";
        case ConfigurationProperty::RESULT_RENDERER:
            return "TableResultRenderer";
    }
    return "";
}

} // namespace javatb
