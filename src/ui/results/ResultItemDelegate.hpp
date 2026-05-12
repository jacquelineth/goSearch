// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// Right-align SIZE and COMPRESSED_SIZE columns.
#pragma once

#include <QStyledItemDelegate>

namespace javatb {

class ResultItemDelegate final : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit ResultItemDelegate(QObject* parent = nullptr);

    void initStyleOption(QStyleOptionViewItem* option,
                         const QModelIndex& index) const override;
};

} // namespace javatb
