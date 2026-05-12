// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "SearchListener.hpp"
#include "UILauncher.hpp"

namespace javatb {

SearchListener::SearchListener(UILauncher* launcher, QObject* parent)
    : QObject(parent), launcher_(launcher)
{
    // Wire signals → UILauncher slots using queued connection (cross-thread safe)
    connect(this, &SearchListener::matchFound,
            launcher_, &UILauncher::onMatchFound,
            Qt::QueuedConnection);
    connect(this, &SearchListener::exploringPath,
            launcher_, &UILauncher::onExploringPath,
            Qt::QueuedConnection);
    connect(this, &SearchListener::searchStarted,
            launcher_, &UILauncher::onSearchStarted,
            Qt::QueuedConnection);
    connect(this, &SearchListener::searchEnded,
            launcher_, &UILauncher::onSearchEnded,
            Qt::QueuedConnection);
}

void SearchListener::newMessage(const SearchEngineEvent& event) {
    switch (event.type) {
        case SearchEngineEvent::Type::START:
            matchCount_ = 0;
            emit searchStarted();
            break;
        case SearchEngineEvent::Type::MATCH:
            ++matchCount_;
            emit matchFound(event.element);
            break;
        case SearchEngineEvent::Type::EXPLORING:
            emit exploringPath(QString::fromStdString(event.element->toString()));
            break;
        case SearchEngineEvent::Type::END:
            emit searchEnded(matchCount_);
            break;
    }
}

} // namespace javatb
