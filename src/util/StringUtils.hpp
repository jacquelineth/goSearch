// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.util.StringUtils
#pragma once

#include <string_view>
#include <charconv>

namespace javatb {

struct StringUtils {
    /// Parse an integer from the string; returns def if parsing fails.
    static int toInt(std::string_view source, int def) noexcept {
        int result = def;
        auto [ptr, ec] = std::from_chars(source.data(), source.data() + source.size(), result);
        return (ec == std::errc{}) ? result : def;
    }

    /// Parse a boolean ("true"/"1" → true; anything else → def on failure).
    static bool toBoolean(std::string_view source, bool def) noexcept {
        if (source == "true"  || source == "TRUE"  || source == "1") return true;
        if (source == "false" || source == "FALSE" || source == "0") return false;
        return def;
    }
};

} // namespace javatb
