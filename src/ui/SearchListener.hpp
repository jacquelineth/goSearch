// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.search.ui.SearchListener
#pragma once

#include "../search/SearchEngineListener.hpp"
#include <QObject>

namespace javatb {

class UILauncher; // forward

/// Bridges SearchEngine events to UILauncher UI updates (thread-safe via Qt queued connections).
class SearchListener final : public QObject, public SearchEngineListener {
    Q_OBJECT
public:
    explicit SearchListener(UILauncher* launcher, QObject* parent = nullptr);

    void newMessage(const SearchEngineEvent& event) override;

signals:
    void matchFound(std::shared_ptr<SearchElement> element);
    void exploringPath(const QString& path);
    void searchStarted();
    void searchEnded(int matchCount);

private:
    UILauncher* launcher_ = nullptr;
    int matchCount_       = 0;
};

} // namespace javatb
