
#include <gtest/gtest.h>

#include <utils/string.h>

using namespace utils;

TEST(strtof_c, EmptyString) {
    char* end = nullptr;
    const char* start = "";
    float r = strtof_c(start, &end);
    EXPECT_EQ(start, end);
    EXPECT_EQ(0.0f, r);
}

TEST(strtof_c, ValidString) {
    char* end = nullptr;
    const char* start = "42.24, independent of the locale";
    float r = strtof_c(start, &end);
    EXPECT_TRUE(end > start);
    EXPECT_FLOAT_EQ(42.24f, r);
}
