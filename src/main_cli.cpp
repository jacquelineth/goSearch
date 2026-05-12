// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.search.Search (CLI entry point)
#include "search/SearchEngine.hpp"
#include "search/SearchEngineListener.hpp"

#include <atomic>
#include <print>
#include <string>
#include <vector>

using namespace javatb;

namespace {

class CliListener final : public SearchEngineListener {
public:
    void newMessage(const SearchEngineEvent& ev) override {
        switch (ev.type) {
            case SearchEngineEvent::Type::MATCH:
                ++matchCount_;
                std::println("{}", ev.element->toString());
                break;
            case SearchEngineEvent::Type::END:
                std::println("--- {} match(es) found ---", matchCount_.load());
                break;
            default:
                break;
        }
    }

    [[nodiscard]] int matchCount() const noexcept { return matchCount_.load(); }

private:
    std::atomic<int> matchCount_{0};
};

} // anonymous namespace

int main(int argc, char** argv) {
    if (argc < 3) {
        std::println(stderr, "Usage: find4j-cli <rootPath> <filePattern> [<searchString>]");
        return 1;
    }

    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) args.emplace_back(argv[i]);

    try {
        SearchEngine engine(args);
        CliListener listener;
        engine.addSearchEngineListener(&listener);
        engine.search();
        return 0;
    } catch (const std::exception& e) {
        std::println(stderr, "Error: {}", e.what());
        return 2;
    }
}
