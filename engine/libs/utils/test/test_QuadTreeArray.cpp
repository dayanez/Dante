
#include <gtest/gtest.h>

#include <utils/QuadTree.h>

using namespace utils;

TEST(QuadTreeArrayTest, TraversalDFS) {

    using QuadTree = QuadTreeArray<bool, 4>;
    QuadTree qt;
    QuadTree::NodeId indices[qt.size()];

    QuadTree::traverse(0, 0,
            [&](auto const& curr) -> QuadTree::TraversalResult {
                size_t i = QuadTreeUtils::index(curr.l, curr.code);
                indices[i] = curr;
                return QuadTree::TraversalResult::RECURSE;
            });

    size_t i = 0;
    for (size_t y = 0; y < QuadTree::height(); y++) {
        for (size_t x = 0; x < (1 << 2 * y); x++, i++) {
            EXPECT_EQ(indices[i].l, y);
            EXPECT_EQ(indices[i].code, x);
        }
    }
}
