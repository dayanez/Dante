
#include <math/fast.h>
#include <math/scalar.h>

#include <gtest/gtest.h>

using namespace dante::math;

class FastTest : public testing::Test {
protected:
};

TEST_F(FastTest, Trig) {
    constexpr float sqrt1_2f = (float) F_SQRT1_2;
    constexpr double sqrt1_2d = F_SQRT1_2;
    constexpr float abs_error = 0.002f; // 0.2%

    EXPECT_FLOAT_EQ( 0.0f,      fast::sin<float>(-F_PI));
    EXPECT_NEAR    (-sqrt1_2f,  fast::sin<float>(-F_PI_2 - F_PI_4), abs_error);
    EXPECT_FLOAT_EQ(-1.0f,      fast::sin<float>(-F_PI_2));
    EXPECT_NEAR    (-sqrt1_2f,  fast::sin<float>(-F_PI_4), abs_error);
    EXPECT_FLOAT_EQ( 0.0f,      fast::sin<float>(0.0));
    EXPECT_NEAR    ( sqrt1_2f,  fast::sin<float>(F_PI_4), abs_error);
    EXPECT_FLOAT_EQ( 1.0f,      fast::sin<float>(F_PI_2));
    EXPECT_NEAR    ( sqrt1_2f,  fast::sin<float>(F_PI_2 + F_PI_4), abs_error);
    EXPECT_FLOAT_EQ( 0.0f,      fast::sin<float>(F_PI));

    EXPECT_FLOAT_EQ(-1.0f,      fast::cos<float>(-F_PI));
    EXPECT_NEAR    (-sqrt1_2f,  fast::cos<float>(-F_PI_2 - F_PI_4), abs_error);
    EXPECT_FLOAT_EQ( 0.0f,      fast::cos<float>(-F_PI_2));
    EXPECT_NEAR    (sqrt1_2f,   fast::cos<float>(-F_PI_4), abs_error);
    EXPECT_FLOAT_EQ( 1.0f,      fast::cos<float>(0.0));
    EXPECT_NEAR    (sqrt1_2f,   fast::cos<float>(F_PI_4), abs_error);
    EXPECT_FLOAT_EQ( 0.0f,      fast::cos<float>(F_PI_2));
    EXPECT_NEAR    (-sqrt1_2f,  fast::cos<float>(F_PI_2 + F_PI_4), abs_error);
    EXPECT_FLOAT_EQ(-1.0f,      fast::cos<float>(F_PI));

    EXPECT_FLOAT_EQ( 0.0f,      fast::sin<double>(-F_PI));
    EXPECT_NEAR    (-sqrt1_2d,  fast::sin<double>(-F_PI_2 - F_PI_4), abs_error);
    EXPECT_FLOAT_EQ(-1.0f,      fast::sin<double>(-F_PI_2));
    EXPECT_NEAR    (-sqrt1_2d,  fast::sin<double>(-F_PI_4), abs_error);
    EXPECT_FLOAT_EQ( 0.0f,      fast::sin<double>(0.0));
    EXPECT_NEAR    ( sqrt1_2d,  fast::sin<double>(F_PI_4), abs_error);
    EXPECT_FLOAT_EQ( 1.0f,      fast::sin<double>(F_PI_2));
    EXPECT_NEAR    ( sqrt1_2d,  fast::sin<double>(F_PI_2 + F_PI_4), abs_error);
    EXPECT_FLOAT_EQ( 0.0f,      fast::sin<double>(F_PI));

    EXPECT_FLOAT_EQ(-1.0f,      fast::cos<double>(-F_PI));
    EXPECT_NEAR    (-sqrt1_2d,  fast::cos<double>(-F_PI_2 - F_PI_4), abs_error);
    EXPECT_FLOAT_EQ( 0.0f,      fast::cos<double>(-F_PI_2));
    EXPECT_NEAR    (sqrt1_2d,   fast::cos<double>(-F_PI_4), abs_error);
    EXPECT_FLOAT_EQ( 1.0f,      fast::cos<double>(0.0));
    EXPECT_NEAR    (sqrt1_2d,   fast::cos<double>(F_PI_4), abs_error);
    EXPECT_FLOAT_EQ( 0.0f,      fast::cos<double>(F_PI_2));
    EXPECT_NEAR    (-sqrt1_2d,  fast::cos<double>(F_PI_2 + F_PI_4), abs_error);
    EXPECT_FLOAT_EQ(-1.0f,      fast::cos<double>(F_PI));
}
