// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "SortedPersistedAutoCompleter.hpp"
#include "../util/Configuration.hpp"
#include "../util/PreferencesManager.hpp"

#include <algorithm>
#include <format>

namespace javatb {

SortedPersistedAutoCompleter::SortedPersistedAutoCompleter(
    QLineEdit* field, const std::string& persistKey, QObject* parent)
    : AutoCompleter(field, parent)
    , persistKey_(persistKey)
{
    maxItems_ = Configuration::instance().getInt(ConfigurationProperty::HISTORY_SIZE, 10);
    PreferencesManager::registerComponent(persistKey_, this);
}

bool SortedPersistedAutoCompleter::updateListData() {
    QString current = field_->text();
    // Add the current text as a candidate if not already present
    if (!current.isEmpty()) {
        // Filter sorted_ for items starting with current (case-insensitive)
        QStringList suggestions;
        for (const auto& item : sorted_) {
            if (item.startsWith(current, Qt::CaseInsensitive))
                suggestions << item;
        }
        model_->setStringList(suggestions);
    } else {
        QStringList all;
        for (const auto& item : sorted_) all << item;
        model_->setStringList(all);
    }
    return !model_->stringList().isEmpty();
}

void SortedPersistedAutoCompleter::onItemAccepted(const QString& item) {
    addItem(item);
    field_->setText(item);
}

QStringList SortedPersistedAutoCompleter::getItems() const {
    QStringList list;
    for (const auto& item : sorted_) list << item;
    return list;
}

void SortedPersistedAutoCompleter::addItem(const QString& item) {
    if (item.isEmpty()) return;
    // Remove if already present (dedup)
    std::erase(items_, item);
    std::erase(sorted_, item);

    items_.push_back(item);
    if (static_cast<int>(items_.size()) > maxItems_)
        items_.erase(items_.begin());

    // Rebuild sorted_ from items_
    sorted_ = items_;
    std::ranges::sort(sorted_);
}

// ── Persistable ──────────────────────────────────────────────────────────────

std::map<std::string, std::string> SortedPersistedAutoCompleter::getValues() const {
    std::map<std::string, std::string> m;
    for (int i = 0; i < static_cast<int>(items_.size()); ++i)
        m[std::format("item{}", i)] = items_[static_cast<std::size_t>(i)].toStdString();
    m["count"] = std::to_string(items_.size());
    return m;
}

void SortedPersistedAutoCompleter::setValues(const std::map<std::string, std::string>& values) {
    items_.clear();
    sorted_.clear();
    auto cit = values.find("count");
    int count = (cit != values.end()) ? std::stoi(cit->second) : 0;
    for (int i = 0; i < count; ++i) {
        auto it = values.find(std::format("item{}", i));
        if (it != values.end())
            addItem(QString::fromStdString(it->second));
    }
}

} // namespace javatb
