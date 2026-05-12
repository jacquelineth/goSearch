// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "DeleteAction.hpp"
#include "../../../../util/FileUtils.hpp"
#include <filesystem>
#include <QMessageBox>
namespace javatb {
DeleteAction::DeleteAction(ResultRenderer* r, QObject* parent)
    : AbstractResultAction("Delete", r, parent)
{
    connect(this, &QAction::triggered, this, [this]() {
        auto sel = getSelection();
        // Cannot delete archive entries
        for (auto& elt : sel) {
            if (elt->isArchiveEntry()) return;
        }
        if (sel.isEmpty()) return;
        auto resp = QMessageBox::question(nullptr, "Delete",
            QString("Delete %1 item(s)? This cannot be undone.").arg(sel.size()),
            QMessageBox::Yes | QMessageBox::No);
        if (resp != QMessageBox::Yes) return;
        for (auto& elt : sel)
            FileUtils::deleteRecursive(std::filesystem::path(elt->toString()));
    });
}
} // namespace javatb
