// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "TablePersistenceHandler.hpp"
#include "../../util/PreferencesManager.hpp"
#include <format>

namespace javatb {

TablePersistenceHandler::TablePersistenceHandler(const std::string& key, QHeaderView* header)
    : header_(header)
{
    PreferencesManager::registerComponent(key, this);
}

std::map<std::string, std::string> TablePersistenceHandler::getValues() const {
    std::map<std::string, std::string> m;
    for (int i = 0; i < header_->count(); ++i)
        m[std::format("col{}", i)] = std::to_string(header_->sectionSize(i));
    return m;
}

void TablePersistenceHandler::setValues(const std::map<std::string, std::string>& values) {
    for (int i = 0; i < header_->count(); ++i) {
        auto it = values.find(std::format("col{}", i));
        if (it != values.end()) {
            int w = std::stoi(it->second);
            if (w > 0) header_->resizeSection(i, w);
        }
    }
}

} // namespace javatb
