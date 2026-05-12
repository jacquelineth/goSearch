// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.search.ui.results.TablePersistenceHandler
#pragma once

#include "../../util/Persistable.hpp"
#include <QHeaderView>
#include <string>

namespace javatb {

/// Persists and restores QHeaderView column widths.
class TablePersistenceHandler final : public Persistable {
public:
    TablePersistenceHandler(const std::string& key, QHeaderView* header);

    [[nodiscard]] std::map<std::string, std::string> getValues() const override;
    void setValues(const std::map<std::string, std::string>& values) override;

private:
    QHeaderView* header_;
};

} // namespace javatb
