// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// Base class for context menu actions on search results.
#pragma once

#include <QAction>
#include <QList>
#include <memory>

#include "../../../search/SearchElement.hpp"

namespace javatb {

class ResultRenderer; // forward

class AbstractResultAction : public QAction {
    Q_OBJECT
public:
    explicit AbstractResultAction(const QString& text, ResultRenderer* renderer,
                                  QObject* parent = nullptr);

    /// Get currently selected elements from the renderer.
    [[nodiscard]] QList<std::shared_ptr<SearchElement>> getSelection() const;

protected:
    ResultRenderer* renderer_ = nullptr;
};

} // namespace javatb
