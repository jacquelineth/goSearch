// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.util.Persistable
#pragma once

#include <map>
#include <string>

namespace javatb {

/// Interface for components that persist their state as key/value pairs.
class Persistable {
public:
    virtual ~Persistable() = default;

    /// Export current state as string key→value pairs.
    [[nodiscard]] virtual std::map<std::string, std::string> getValues() const = 0;

    /// Restore state from previously saved key→value pairs.
    virtual void setValues(const std::map<std::string, std::string>& values) = 0;
};

} // namespace javatb
