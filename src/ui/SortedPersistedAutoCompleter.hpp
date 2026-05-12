// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.search.ui.SortedPersistedAutoCompleter
#pragma once

#include "AutoCompleter.hpp"
#include "../util/Persistable.hpp"

#include <QStringList>
#include <string>
#include <vector>

namespace javatb {

/// AutoCompleter that keeps a sorted, size-bounded, persisted history.
class SortedPersistedAutoCompleter final : public AutoCompleter, public Persistable {
    Q_OBJECT
public:
    explicit SortedPersistedAutoCompleter(QLineEdit* field,
                                          const std::string& persistKey,
                                          QObject* parent = nullptr);

    // AutoCompleter
    bool updateListData()               override;
    void onItemAccepted(const QString&) override;
    [[nodiscard]] QStringList getItems() const override;

    // Persistable
    [[nodiscard]] std::map<std::string, std::string> getValues() const override;
    void setValues(const std::map<std::string, std::string>& values) override;

private:
    void addItem(const QString& item);

    std::vector<QString> items_;      ///< insertion-order list
    std::vector<QString> sorted_;     ///< sorted copy for binary search
    std::string          persistKey_;
    int                  maxItems_ = 10;
};

} // namespace javatb
