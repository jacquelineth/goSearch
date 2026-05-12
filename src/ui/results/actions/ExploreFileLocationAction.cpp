// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "ExploreFileLocationAction.hpp"
#include "../../../../util/FileUtils.hpp"
namespace javatb {
ExploreFileLocationAction::ExploreFileLocationAction(ResultRenderer* r, QObject* parent)
    : AbstractResultAction("Show in file manager", r, parent)
{
    connect(this, &QAction::triggered, this, [this]() {
        for (auto& elt : getSelection())
            FileUtils::exploreSearchElement(*elt);
    });
}
} // namespace javatb
