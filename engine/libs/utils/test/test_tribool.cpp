
#include <utils/tribool.h>

#include <gtest/gtest.h>

using namespace utils;

TEST(TriboolTest, ConstructorsAndObservers) {
    tribool t1;
    EXPECT_TRUE(t1.is_indeterminate());
    EXPECT_FALSE(t1.is_true());
    EXPECT_FALSE(t1.is_false());

    tribool t2(true);
    EXPECT_TRUE(t2.is_true());
    EXPECT_FALSE(t2.is_indeterminate());
    EXPECT_FALSE(t2.is_false());

    tribool t3(false);
    EXPECT_TRUE(t3.is_false());
    EXPECT_FALSE(t3.is_true());
    EXPECT_FALSE(t3.is_indeterminate());

    tribool t4(tribool::kTrue);
    EXPECT_TRUE(t4.is_true());

    tribool t5(tribool::kFalse);
    EXPECT_TRUE(t5.is_false());

    tribool t6(tribool::kIndeterminate);
    EXPECT_TRUE(t6.is_indeterminate());
}

TEST(TriboolTest, LogicalNot) {
    EXPECT_TRUE((!tribool(true)).is_false());
    EXPECT_TRUE((!tribool(false)).is_true());
    EXPECT_TRUE((!tribool(tribool::kIndeterminate)).is_indeterminate());
}

TEST(TriboolTest, LogicalAnd) {
    tribool T(true);
    tribool F(false);
    tribool I(tribool::kIndeterminate);

    EXPECT_TRUE((T && T).is_true());
    EXPECT_TRUE((T && F).is_false());
    EXPECT_TRUE((F && T).is_false());
    EXPECT_TRUE((F && F).is_false());

    EXPECT_TRUE((T && I).is_indeterminate());
    EXPECT_TRUE((I && T).is_indeterminate());

    EXPECT_TRUE((F && I).is_false());
    EXPECT_TRUE((I && F).is_false());

    EXPECT_TRUE((I && I).is_indeterminate());
}

TEST(TriboolTest, LogicalOr) {
    tribool T(true);
    tribool F(false);
    tribool I(tribool::kIndeterminate);

    EXPECT_TRUE((T || T).is_true());
    EXPECT_TRUE((T || F).is_true());
    EXPECT_TRUE((F || T).is_true());
    EXPECT_TRUE((F || F).is_false());

    EXPECT_TRUE((T || I).is_true());
    EXPECT_TRUE((I || T).is_true());

    EXPECT_TRUE((F || I).is_indeterminate());
    EXPECT_TRUE((I || F).is_indeterminate());

    EXPECT_TRUE((I || I).is_indeterminate());
}

TEST(TriboolTest, Equality) {
    tribool T(true);
    tribool F(false);
    tribool I(tribool::kIndeterminate);

    EXPECT_TRUE(T == tribool(true));
    EXPECT_TRUE(F == tribool(false));
    EXPECT_TRUE(I == tribool(tribool::kIndeterminate));

    EXPECT_TRUE(T != F);
    EXPECT_TRUE(T != I);
    EXPECT_TRUE(F != I);
}
