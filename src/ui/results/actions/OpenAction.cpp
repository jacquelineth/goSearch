// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "OpenAction.hpp"
#include "../../../../util/FileUtils.hpp"
namespace javatb {
OpenAction::OpenAction(ResultRenderer* r, QObject* parent)
    : AbstractResultAction("Open", r, parent)
{
    QFont f = font(); f.setBold(true); setFont(f);
    connect(this, &QAction::triggered, this, [this]() {
        for (auto& elt : getSelection())
            FileUtils::openSearchElement(*elt);
    });
}
} // namespace javatb
