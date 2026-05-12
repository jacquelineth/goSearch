// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "TableResultRenderer.hpp"
#include "ResultItemDelegate.hpp"
#include "../../util/FileUtils.hpp"
#include "actions/OpenAction.hpp"
#include "actions/EditAction.hpp"
#include "actions/CopyToClipBoardAction.hpp"
#include "actions/DeleteAction.hpp"
#include "actions/ExploreFileLocationAction.hpp"

#include <QHeaderView>
#include <QMenu>
#include <QScrollArea>

namespace javatb {

TableResultRenderer::TableResultRenderer(QObject* parent) : QObject(parent) {
    model_ = new ResultTableModel(this);
    proxy_ = new QSortFilterProxyModel(this);
    proxy_->setSourceModel(model_);
    // Sort SIZE/COMPRESSED_SIZE numerically using UserRole (raw long long)
    proxy_->setSortRole(Qt::UserRole);

    table_ = new QTableView();
    table_->setModel(proxy_);
    table_->setSortingEnabled(true);
    table_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setShowGrid(false);
    table_->setItemDelegate(new ResultItemDelegate(table_));
    table_->horizontalHeader()->setStretchLastSection(false);
    table_->verticalHeader()->hide();
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setAlternatingRowColors(true);
    table_->setContextMenuPolicy(Qt::CustomContextMenu);

    // Default column widths
    auto* hdr = table_->horizontalHeader();
    hdr->resizeSection(ResultTableModel::PATH,            500);
    hdr->resizeSection(ResultTableModel::SIZE,             60);
    hdr->resizeSection(ResultTableModel::COMPRESSED_SIZE,  60);
    hdr->resizeSection(ResultTableModel::DATE,            120);

    persistenceHandler_ = std::make_unique<TablePersistenceHandler>("result.table", hdr);

    // Stretch PATH column when the table is resized
    connect(table_, &QTableView::sizeHintChanged, this, [this]() {
        auto* h = table_->horizontalHeader();
        int sum = 0;
        for (int i = 1; i < model_->columnCount(); ++i)
            sum += h->sectionSize(i);
        int pathW = table_->width() - sum;
        if (pathW > 0) h->resizeSection(ResultTableModel::PATH, pathW);
    });

    // Double-click → Open
    connect(table_, &QTableView::doubleClicked, this, [this](const QModelIndex&) {
        for (auto& elt : selectedElements())
            javatb::FileUtils::openSearchElement(*elt);
    });

    // Right-click → context menu
    connect(table_, &QWidget::customContextMenuRequested,
            this, [this](const QPoint& pos) { buildContextMenu(table_->mapToGlobal(pos)); });

    // Wrap in a scroll area (the table itself scrolls, but we match the Java layout)
    scrollPane_ = new QScrollArea();
    scrollPane_->setWidget(table_);
    scrollPane_->setWidgetResizable(true);
}

void TableResultRenderer::addElement(std::shared_ptr<SearchElement> element) {
    model_->addElement(std::move(element));
}

std::shared_ptr<SearchElement> TableResultRenderer::elementAt(int row) {
    int srcRow = proxy_->mapToSource(proxy_->index(row, 0)).row();
    return model_->elementAt(srcRow);
}

QList<std::shared_ptr<SearchElement>> TableResultRenderer::selectedElements() {
    QList<std::shared_ptr<SearchElement>> list;
    const auto rows = table_->selectionModel()->selectedRows();
    for (const auto& idx : rows) {
        int srcRow = proxy_->mapToSource(idx).row();
        auto elt = model_->elementAt(srcRow);
        if (elt) list.push_back(elt);
    }
    return list;
}

void TableResultRenderer::clear() { model_->clear(); }

int TableResultRenderer::rowFromLocation(int x, int y) {
    return table_->rowAt(y);
}

void TableResultRenderer::showContextMenu(const QPoint& pos) {
    buildContextMenu(pos);
}

void TableResultRenderer::buildContextMenu(const QPoint& globalPos) {
    if (selectedElements().isEmpty()) return;
    QMenu menu;
    menu.addAction(new OpenAction(this, &menu));
    menu.addAction(new EditAction(this, &menu));
    menu.addSeparator();
    menu.addAction(new CopyToClipBoardAction(this, &menu));
    menu.addSeparator();
    menu.addAction(new DeleteAction(this, &menu));
    menu.addAction(new ExploreFileLocationAction(this, &menu));
    menu.exec(globalPos);
}

} // namespace javatb
