// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.util.PreferencesManager (backed by QSettings)
#pragma once

#include <string>
#include "Persistable.hpp"

namespace javatb {

/// Manages persistent storage of UI component state via QSettings.
/// Uses the path "JavaTB/Find4j" as the application group.
class PreferencesManager {
public:
    PreferencesManager() = delete;

    /// Register a Persistable and immediately restore any previously saved values.
    static void registerComponent(const std::string& name, Persistable* persistable);

    /// Persist the state of the named component.
    static void save(const std::string& name);

    /// Persist all registered components.
    static void saveAll();
};

} // namespace javatb
