// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.search.ui.results.ResultRenderer
#pragma once

#include <memory>
#include <QList>
#include <QPoint>
#include <QWidget>

#include "../../search/SearchElement.hpp"

namespace javatb {

/// Abstract interface for pluggable search-result views.
class ResultRenderer {
public:
    virtual ~ResultRenderer() = default;

    /// Returns the widget to embed in the main layout.
    [[nodiscard]] virtual QWidget* getWidget() = 0;

    /// Add a matched element to the view.
    virtual void addElement(std::shared_ptr<SearchElement> element) = 0;

    /// Get the element at a given (view) row index.
    [[nodiscard]] virtual std::shared_ptr<SearchElement> elementAt(int row) = 0;

    /// Get all currently selected elements.
    [[nodiscard]] virtual QList<std::shared_ptr<SearchElement>> selectedElements() = 0;

    /// Clear all results.
    virtual void clear() = 0;

    /// Return the row at the given viewport coordinate, or -1 if none.
    [[nodiscard]] virtual int rowFromLocation(int x, int y) = 0;

    /// Display a context menu at the given viewport position.
    virtual void showContextMenu(const QPoint& pos) = 0;
};

} // namespace javatb
