// C++ port - Configuration unit tests
#include <gtest/gtest.h>
#include "../src/util/Configuration.hpp"
#include "../src/util/ConfigurationProperty.hpp"

using namespace javatb;

TEST(Configuration, GetStringReturnsDefaultWhenFileAbsent) {
    Configuration::instance().reset(); // might fail to load file in test CWD; uses defaults
    auto val = Configuration::instance().getString(ConfigurationProperty::HISTORY_SIZE);
    EXPECT_FALSE(val.empty());
    // Default is "10"
    EXPECT_EQ(val, "10");
}

TEST(Configuration, GetArrayPropertyReturnsTokens) {
    auto tokens = Configuration::instance().getArrayProperty(ConfigurationProperty::ZIP_EXTENSIONS);
    EXPECT_FALSE(tokens.empty());
    bool hasZip = std::ranges::any_of(tokens, [](const std::string& t){ return t == "zip"; });
    EXPECT_TRUE(hasZip);
}

TEST(Configuration, GetIntHistorySize) {
    int v = Configuration::instance().getInt(ConfigurationProperty::HISTORY_SIZE, 5);
    EXPECT_GT(v, 0);
}

TEST(Configuration, DefaultResultRenderer) {
    auto val = Configuration::instance().getString(ConfigurationProperty::RESULT_RENDERER);
    EXPECT_FALSE(val.empty());
}

TEST(ConfigurationProperty, KeyNames) {
    EXPECT_EQ(propertyKey(ConfigurationProperty::ZIP_EXTENSIONS),
              "org.javatb.search.zip.extensions");
    EXPECT_EQ(propertyKey(ConfigurationProperty::HISTORY_SIZE),
              "org.javatb.history.size");
    EXPECT_EQ(propertyKey(ConfigurationProperty::RESULT_RENDERER),
              "org.javatb.renderer.class");
}

TEST(ConfigurationProperty, Defaults) {
    EXPECT_EQ(propertyDefault(ConfigurationProperty::HISTORY_SIZE), "10");
    EXPECT_EQ(propertyDefault(ConfigurationProperty::RESULT_RENDERER), "TableResultRenderer");
    auto extDefault = propertyDefault(ConfigurationProperty::ZIP_EXTENSIONS);
    EXPECT_TRUE(extDefault.find("zip") != std::string_view::npos);
}
