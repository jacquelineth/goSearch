// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.search.ui.FramePersistenceHandler
#pragma once

#include "../util/Persistable.hpp"
#include <QMainWindow>
#include <string>

namespace javatb {

/// Persists and restores a QMainWindow's geometry and state.
class FramePersistenceHandler final : public Persistable {
public:
    /// Registers and immediately restores saved window geometry.
    FramePersistenceHandler(const std::string& key, QMainWindow* window);

    [[nodiscard]] std::map<std::string, std::string> getValues() const override;
    void setValues(const std::map<std::string, std::string>& values) override;

private:
    QMainWindow* window_;
};

} // namespace javatb
