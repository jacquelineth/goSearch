// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "AutoCompleter.hpp"
#include <QAbstractItemView>
#include <QKeyEvent>

namespace javatb {

AutoCompleter::AutoCompleter(QLineEdit* field, QObject* parent)
    : QObject(parent)
    , field_(field)
{
    model_    = new QStringListModel(this);
    completer_= new QCompleter(model_, this);
    completer_->setCompletionMode(QCompleter::PopupCompletion);
    completer_->setCaseSensitivity(Qt::CaseInsensitive);
    field_->setCompleter(completer_);

    // On every text change rebuild the list
    connect(field_, &QLineEdit::textEdited, this, [this](const QString&) {
        updateListData();
        if (!getItems().isEmpty())
            completer_->complete();
    });

    // When user activates a completion
    connect(completer_, qOverload<const QString&>(&QCompleter::activated),
            this, &AutoCompleter::onItemAccepted);
}

} // namespace javatb
