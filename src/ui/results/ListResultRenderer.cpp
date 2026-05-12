// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "ListResultRenderer.hpp"
#include "../../util/FileUtils.hpp"
#include "actions/OpenAction.hpp"
#include "actions/EditAction.hpp"
#include "actions/CopyToClipBoardAction.hpp"
#include "actions/DeleteAction.hpp"
#include "actions/ExploreFileLocationAction.hpp"

#include <QMenu>

namespace javatb {

ListResultRenderer::ListResultRenderer(QObject* parent) : QObject(parent) {
    listModel_ = new QStringListModel(this);
    listView_  = new QListView();
    listView_->setModel(listModel_);
    listView_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    listView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listView_->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(listView_, &QListView::doubleClicked, this, [this](const QModelIndex& idx) {
        auto elt = elementAt(idx.row());
        if (elt) FileUtils::openSearchElement(*elt);
    });

    connect(listView_, &QWidget::customContextMenuRequested,
            this, [this](const QPoint& pos) {
        if (selectedElements().isEmpty()) return;
        QMenu menu;
        menu.addAction(new OpenAction(this, &menu));
        menu.addAction(new EditAction(this, &menu));
        menu.addSeparator();
        menu.addAction(new CopyToClipBoardAction(this, &menu));
        menu.addSeparator();
        menu.addAction(new DeleteAction(this, &menu));
        menu.addAction(new ExploreFileLocationAction(this, &menu));
        menu.exec(listView_->mapToGlobal(pos));
    });

    scrollPane_ = new QScrollArea();
    scrollPane_->setWidget(listView_);
    scrollPane_->setWidgetResizable(true);
}

void ListResultRenderer::addElement(std::shared_ptr<SearchElement> element) {
    elements_.push_back(element);
    QStringList sl = listModel_->stringList();
    sl << QString::fromStdString(element->toString());
    listModel_->setStringList(sl);
}

std::shared_ptr<SearchElement> ListResultRenderer::elementAt(int row) {
    if (row < 0 || row >= static_cast<int>(elements_.size())) return {};
    return elements_[static_cast<std::size_t>(row)];
}

QList<std::shared_ptr<SearchElement>> ListResultRenderer::selectedElements() {
    QList<std::shared_ptr<SearchElement>> list;
    const auto idxs = listView_->selectionModel()->selectedIndexes();
    for (const auto& idx : idxs) {
        auto elt = elementAt(idx.row());
        if (elt) list.push_back(elt);
    }
    return list;
}

void ListResultRenderer::clear() {
    elements_.clear();
    listModel_->setStringList({});
}

int ListResultRenderer::rowFromLocation(int x, int y) {
    auto idx = listView_->indexAt({x, y});
    return idx.isValid() ? idx.row() : -1;
}

void ListResultRenderer::showContextMenu(const QPoint& pos) {
    // handled by customContextMenuRequested signal
    Q_UNUSED(pos)
}

} // namespace javatb
