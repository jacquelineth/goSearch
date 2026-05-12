// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.search.SearchEngineEvent
#pragma once

#include <memory>
#include "SearchElement.hpp"

namespace javatb {

struct SearchEngineEvent {
    enum class Type {
        EXPLORING, ///< Entering a folder or archive
        MATCH,     ///< A file/entry matched all criteria
        START,     ///< Search is beginning
        END,       ///< Search has completed (or was cancelled)
    };

    Type type;
    std::shared_ptr<SearchElement> element;

    SearchEngineEvent(Type t, std::shared_ptr<SearchElement> elt)
        : type(t), element(std::move(elt)) {}
};

} // namespace javatb
