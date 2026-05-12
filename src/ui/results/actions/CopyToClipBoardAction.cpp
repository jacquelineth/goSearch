// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "CopyToClipBoardAction.hpp"
#include <QApplication>
#include <QClipboard>
namespace javatb {
CopyToClipBoardAction::CopyToClipBoardAction(ResultRenderer* r, QObject* parent)
    : AbstractResultAction("Copy to clipboard", r, parent)
{
    connect(this, &QAction::triggered, this, [this]() {
        QStringList paths;
        for (auto& elt : getSelection())
            paths << QString::fromStdString(elt->toString());
        QApplication::clipboard()->setText(paths.join('\n'));
    });
}
} // namespace javatb
