// C++ port - SearchEngine unit tests
#include <gtest/gtest.h>
#include "../src/search/SearchEngine.hpp"
#include "../src/search/SearchEngineListener.hpp"

using namespace javatb;

TEST(SearchEngine, ConstructWithTwoArgs) {
    EXPECT_NO_THROW(SearchEngine engine({".", "*.txt"}));
}

TEST(SearchEngine, ConstructWithThreeArgs) {
    EXPECT_NO_THROW(SearchEngine engine({".", "*.java", "public class"}));
}

TEST(SearchEngine, DefaultNotStopped) {
    SearchEngine engine({".", "*"});
    EXPECT_FALSE(engine.isStopped());
}

TEST(SearchEngine, SetStoppedFlag) {
    SearchEngine engine({".", "*"});
    engine.setStopped(true);
    EXPECT_TRUE(engine.isStopped());
    engine.setStopped(false);
    EXPECT_FALSE(engine.isStopped());
}

TEST(SearchEngine, FiresStartAndEndEvents) {
    struct Recorder : SearchEngineListener {
        std::vector<SearchEngineEvent::Type> types;
        void newMessage(const SearchEngineEvent& ev) override {
            types.push_back(ev.type);
        }
    };

    SearchEngine engine({".", "*.nonexistent_extension_xyz"});
    Recorder rec;
    engine.addSearchEngineListener(&rec);
    engine.search();

    ASSERT_GE(rec.types.size(), 2u);
    EXPECT_EQ(rec.types.front(), SearchEngineEvent::Type::START);
    EXPECT_EQ(rec.types.back(),  SearchEngineEvent::Type::END);
}

TEST(SearchEngine, AddRemoveListener) {
    struct Counter : SearchEngineListener {
        int count = 0;
        void newMessage(const SearchEngineEvent&) override { ++count; }
    };

    SearchEngine engine({".", "*"});
    Counter c;
    engine.addSearchEngineListener(&c);
    engine.removeSearchEngineListener(&c);
    engine.search();
    // After removing, counter should not have been called for MATCH events
    // START and END are guaranteed — but counter was removed so count stays 0
    EXPECT_EQ(c.count, 0);
}
