// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.search.ui.results.ListResultRenderer
#pragma once

#include "ResultRenderer.hpp"

#include <QListView>
#include <QScrollArea>
#include <QStringListModel>
#include <memory>
#include <vector>

namespace javatb {

class ListResultRenderer final : public QObject, public ResultRenderer {
    Q_OBJECT
public:
    explicit ListResultRenderer(QObject* parent = nullptr);

    [[nodiscard]] QWidget* getWidget()            override { return scrollPane_; }
    void addElement(std::shared_ptr<SearchElement>) override;
    [[nodiscard]] std::shared_ptr<SearchElement> elementAt(int row) override;
    [[nodiscard]] QList<std::shared_ptr<SearchElement>> selectedElements() override;
    void clear()                                  override;
    [[nodiscard]] int rowFromLocation(int x, int y) override;
    void showContextMenu(const QPoint& pos)       override;

private:
    QListView*   listView_   = nullptr;
    QScrollArea* scrollPane_ = nullptr;
    QStringListModel* listModel_ = nullptr;
    std::vector<std::shared_ptr<SearchElement>> elements_;
};

} // namespace javatb
