// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "PreferencesManager.hpp"

#include <map>
#include <string>
#include <QSettings>
#include <QString>

namespace javatb {

namespace {
struct Registry {
    std::map<std::string, Persistable*> components;

    static Registry& get() {
        static Registry inst;
        return inst;
    }

    QSettings& settings() {
        static QSettings qs("JavaTB", "Find4j");
        return qs;
    }
};
} // anonymous namespace

void PreferencesManager::registerComponent(const std::string& name, Persistable* persistable) {
    auto& reg = Registry::get();
    reg.components[name] = persistable;

    // Restore saved values immediately
    auto& qs = reg.settings();
    qs.beginGroup(QString::fromStdString(name));
    const auto keys = qs.childKeys();
    if (!keys.isEmpty()) {
        std::map<std::string, std::string> values;
        for (const auto& k : keys)
            values[k.toStdString()] = qs.value(k).toString().toStdString();
        persistable->setValues(values);
    }
    qs.endGroup();
}

void PreferencesManager::save(const std::string& name) {
    auto& reg = Registry::get();
    auto it = reg.components.find(name);
    if (it == reg.components.end()) return;

    auto& qs = reg.settings();
    qs.beginGroup(QString::fromStdString(name));
    for (auto& [key, val] : it->second->getValues())
        qs.setValue(QString::fromStdString(key), QString::fromStdString(val));
    qs.endGroup();
    qs.sync();
}

void PreferencesManager::saveAll() {
    auto& reg = Registry::get();
    auto& qs  = reg.settings();
    for (auto& [name, persistable] : reg.components) {
        qs.beginGroup(QString::fromStdString(name));
        for (auto& [key, val] : persistable->getValues())
            qs.setValue(QString::fromStdString(key), QString::fromStdString(val));
        qs.endGroup();
    }
    qs.sync();
}

} // namespace javatb
