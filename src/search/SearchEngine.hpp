// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.search.SearchEngine
#pragma once

#include <atomic>
#include <deque>
#include <memory>
#include <mutex>
#include <regex>
#include <string>
#include <thread>
#include <vector>

#include "SearchElement.hpp"
#include "SearchEngineEvent.hpp"
#include "SearchEngineListener.hpp"

namespace javatb {

/// Event-driven recursive file/archive search engine.
/// Runs synchronously; call search() from a background thread.
/// Thread-safe stop via setStopped(true).
class SearchEngine {
public:
    /// @param args  { rootPath, filePattern [, contentSearchTerm] }
    explicit SearchEngine(std::vector<std::string> args);

    // Not copyable or moveable (owns mutex + atomic)
    SearchEngine(const SearchEngine&)            = delete;
    SearchEngine& operator=(const SearchEngine&) = delete;

    /// Start the search (blocking).  Returns when complete or stopped.
    void search();

    // ── Listener management ──────────────────────────────────────────────────
    void addSearchEngineListener(SearchEngineListener* listener);
    void removeSearchEngineListener(SearchEngineListener* listener);

    // ── Stop flag ────────────────────────────────────────────────────────────
    [[nodiscard]] bool isStopped() const noexcept { return stopped_.load(); }
    void setStopped(bool v) noexcept { stopped_.store(v); }

    /// Thread-safe snapshot of the current traversal path.
    [[nodiscard]] std::vector<std::shared_ptr<SearchElement>> getCurrentPath() const;

private:
    // ── Internal search helpers ──────────────────────────────────────────────
    void searchPath(const std::filesystem::path& root);
    void searchArchive(std::span<const std::byte> data,
                       const std::vector<std::shared_ptr<SearchElement>>& currentPath);

    /// Build a case-insensitive regex from a wildcard pattern (* and ?).
    static std::regex wildcardToRegex(const std::string& pattern);

    /// Fire an event to all registered listeners.
    void fireEvent(SearchEngineEvent::Type type,
                   std::shared_ptr<SearchElement> element);

    // ── Path stack management (mutex-protected) ──────────────────────────────
    void pushPath(std::shared_ptr<SearchElement> elt);
    void popPath();

    // ── Fields ───────────────────────────────────────────────────────────────
    std::string   rootPath_;
    std::regex    fileRegex_;
    bool          hasContentPattern_ = false;
    std::regex    contentRegex_;

    std::vector<SearchEngineListener*> listeners_;

    mutable std::mutex                              pathMutex_;
    std::deque<std::shared_ptr<SearchElement>>      currentPath_;

    std::atomic<bool> stopped_{false};
};

} // namespace javatb
