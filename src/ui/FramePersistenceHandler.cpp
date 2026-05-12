// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "FramePersistenceHandler.hpp"
#include "../util/PreferencesManager.hpp"

#include <QByteArray>
#include <QString>

namespace javatb {

FramePersistenceHandler::FramePersistenceHandler(const std::string& key, QMainWindow* window)
    : window_(window)
{
    PreferencesManager::registerComponent(key, this);
}

std::map<std::string, std::string> FramePersistenceHandler::getValues() const {
    QByteArray geo   = window_->saveGeometry();
    QByteArray state = window_->saveState();
    return {
        {"geometry", geo.toBase64().toStdString()},
        {"state",    state.toBase64().toStdString()},
    };
}

void FramePersistenceHandler::setValues(const std::map<std::string, std::string>& values) {
    auto git = values.find("geometry");
    if (git != values.end()) {
        window_->restoreGeometry(
            QByteArray::fromBase64(QByteArray::fromStdString(git->second)));
    }
    auto sit = values.find("state");
    if (sit != values.end()) {
        window_->restoreState(
            QByteArray::fromBase64(QByteArray::fromStdString(sit->second)));
    }
}

} // namespace javatb
