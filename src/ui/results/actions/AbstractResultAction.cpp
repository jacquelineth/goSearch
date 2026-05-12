// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "AbstractResultAction.hpp"
#include "../ResultRenderer.hpp"

namespace javatb {

AbstractResultAction::AbstractResultAction(const QString& text, ResultRenderer* renderer,
                                            QObject* parent)
    : QAction(text, parent), renderer_(renderer) {}

QList<std::shared_ptr<SearchElement>> AbstractResultAction::getSelection() const {
    return renderer_->selectedElements();
}

} // namespace javatb
