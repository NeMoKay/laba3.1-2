#include "Fixtures.hpp"

TEST_F(AVLTreeFixture, EmptyTreeInitiallyZeroSize) {
    EXPECT_EQ(empty_tree.getSize(), 0);
    EXPECT_TRUE(empty_tree.isEmpty());
}
