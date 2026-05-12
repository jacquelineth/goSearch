// C++ port - StringUtils unit tests
#include <gtest/gtest.h>
#include "../src/util/StringUtils.hpp"

using namespace javatb;

TEST(StringUtils, ToIntValid) {
    EXPECT_EQ(StringUtils::toInt("42", 0), 42);
    EXPECT_EQ(StringUtils::toInt("0",  99), 0);
    EXPECT_EQ(StringUtils::toInt("-7", 0), -7);
}

TEST(StringUtils, ToIntInvalid) {
    EXPECT_EQ(StringUtils::toInt("abc",  5), 5);
    EXPECT_EQ(StringUtils::toInt("",    -1), -1);
    EXPECT_EQ(StringUtils::toInt("1.5",  3), 3);
}

TEST(StringUtils, ToBooleanTrue) {
    EXPECT_TRUE(StringUtils::toBoolean("true",  false));
    EXPECT_TRUE(StringUtils::toBoolean("TRUE",  false));
    EXPECT_TRUE(StringUtils::toBoolean("1",     false));
}

TEST(StringUtils, ToBooleanFalse) {
    EXPECT_FALSE(StringUtils::toBoolean("false", true));
    EXPECT_FALSE(StringUtils::toBoolean("FALSE", true));
    EXPECT_FALSE(StringUtils::toBoolean("0",     true));
}

TEST(StringUtils, ToBooleanDefault) {
    EXPECT_TRUE(StringUtils::toBoolean("maybe", true));
    EXPECT_FALSE(StringUtils::toBoolean("maybe", false));
    EXPECT_FALSE(StringUtils::toBoolean("",     false));
}
