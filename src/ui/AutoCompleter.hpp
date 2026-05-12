// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.search.ui.AutoCompleter
#pragma once

#include <QCompleter>
#include <QLineEdit>
#include <QObject>
#include <QStringList>
#include <QStringListModel>

namespace javatb {

/// Abstract auto-completion decorator for a QLineEdit.
/// Subclasses implement getItems() and onItemAccepted().
class AutoCompleter : public QObject {
    Q_OBJECT
public:
    explicit AutoCompleter(QLineEdit* field, QObject* parent = nullptr);
    ~AutoCompleter() override = default;

    /// Rebuild the completion list from current text; called on every keystroke.
    virtual bool updateListData() = 0;

    /// Called when the user accepts a suggestion.
    virtual void onItemAccepted(const QString& item) = 0;

    /// Return the current list of suggestions.
    [[nodiscard]] virtual QStringList getItems() const = 0;

protected:
    QLineEdit*        field_    = nullptr;
    QCompleter*       completer_= nullptr;
    QStringListModel* model_    = nullptr;
};

} // namespace javatb
