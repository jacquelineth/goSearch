// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.search.SearchEngineListener
#pragma once

#include "SearchEngineEvent.hpp"

namespace javatb {

/// Observer interface for search progress events.
class SearchEngineListener {
public:
    virtual ~SearchEngineListener() = default;
    virtual void newMessage(const SearchEngineEvent& event) = 0;
};

} // namespace javatb
