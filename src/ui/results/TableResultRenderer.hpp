// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.search.ui.results.TableResultRenderer
#pragma once

#include "ResultRenderer.hpp"
#include "ResultTableModel.hpp"
#include "TablePersistenceHandler.hpp"

#include <QHeaderView>
#include <QScrollArea>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <memory>

namespace javatb {

class TableResultRenderer final : public QObject, public ResultRenderer {
    Q_OBJECT
public:
    explicit TableResultRenderer(QObject* parent = nullptr);
    ~TableResultRenderer() override = default;

    // ResultRenderer
    [[nodiscard]] QWidget* getWidget()           override { return scrollPane_; }
    void addElement(std::shared_ptr<SearchElement>) override;
    [[nodiscard]] std::shared_ptr<SearchElement> elementAt(int row) override;
    [[nodiscard]] QList<std::shared_ptr<SearchElement>> selectedElements() override;
    void clear()                                 override;
    [[nodiscard]] int rowFromLocation(int x, int y) override;
    void showContextMenu(const QPoint& pos)      override;

private:
    QTableView*            table_      = nullptr;
    ResultTableModel*      model_      = nullptr;
    QSortFilterProxyModel* proxy_      = nullptr;
    QScrollArea*           scrollPane_ = nullptr;
    std::unique_ptr<TablePersistenceHandler> persistenceHandler_;

    void buildContextMenu(const QPoint& globalPos);
};

} // namespace javatb
