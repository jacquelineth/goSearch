// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "EditAction.hpp"
#include "../../../../util/FileUtils.hpp"
namespace javatb {
EditAction::EditAction(ResultRenderer* r, QObject* parent)
    : AbstractResultAction("Edit", r, parent)
{
    connect(this, &QAction::triggered, this, [this]() {
        for (auto& elt : getSelection())
            FileUtils::editSearchElement(*elt);
    });
}
} // namespace javatb
