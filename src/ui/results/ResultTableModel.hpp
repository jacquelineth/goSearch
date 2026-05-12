// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.search.ui.results.ResultTableModel
#pragma once

#include <chrono>
#include <memory>
#include <vector>

#include <QAbstractTableModel>
#include <QString>
#include <QVariant>

#include "../../search/SearchElement.hpp"

namespace javatb {

class ResultTableModel final : public QAbstractTableModel {
    Q_OBJECT
public:
    // Column indices (mirrors Java constants)
    static constexpr int PATH            = 0;
    static constexpr int SIZE            = 1;
    static constexpr int COMPRESSED_SIZE = 2;
    static constexpr int DATE            = 3;
    static constexpr int ATTRIBUTES      = 4;
    static constexpr int COLUMN_COUNT    = 5;

    explicit ResultTableModel(QObject* parent = nullptr);

    // QAbstractTableModel overrides
    [[nodiscard]] int rowCount(const QModelIndex& parent = {})    const override;
    [[nodiscard]] int columnCount(const QModelIndex& parent = {}) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index,
                                int role = Qt::DisplayRole)       const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                      int role = Qt::DisplayRole) const override;

    // Data management
    void addElement(std::shared_ptr<SearchElement> element);
    [[nodiscard]] std::shared_ptr<SearchElement> elementAt(int row) const;
    void clear();

private:
    [[nodiscard]] static QString formatSize(long long bytes);
    [[nodiscard]] static QString formatDate(const SearchElement& elt);

    std::vector<std::shared_ptr<SearchElement>> results_;
};

} // namespace javatb
