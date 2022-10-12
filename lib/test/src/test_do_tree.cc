#include <gtest/gtest.h>

#include "do_tree.hpp"

template struct red::containers::do_tree<int>;
using do_tree = typename red::containers::do_tree<int>;

TEST (test_do_tree, test_ctor) { do_tree {}; }
TEST (test_do_tree, test_iterator_decrement)
{
    do_tree tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.erase (1);
    tree.erase (7);
    tree.erase (4);

    EXPECT_EQ (*(--tree.find (6)), 5);
    EXPECT_EQ (*(--tree.find (8)), 6);
    EXPECT_EQ (*(--tree.find (10)), 9);
    EXPECT_EQ (*(--tree.find (11)), 10);
    EXPECT_EQ (*(--tree.find (3)), 2);
    EXPECT_EQ ((--tree.find (2)).m_node, nullptr);
}
