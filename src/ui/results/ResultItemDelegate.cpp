// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "ResultItemDelegate.hpp"
#include "ResultTableModel.hpp"

#include <QStyleOptionViewItem>

namespace javatb {

ResultItemDelegate::ResultItemDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

void ResultItemDelegate::initStyleOption(QStyleOptionViewItem* option,
                                          const QModelIndex& index) const {
    QStyledItemDelegate::initStyleOption(option, index);
    int col = index.column();
    if (col == ResultTableModel::SIZE || col == ResultTableModel::COMPRESSED_SIZE)
        option->displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
}

} // namespace javatb
