#include <gtest/gtest.h>

#include "splay_do_tree.hpp"

template struct red::containers::splay_do_tree<int>;
using splay_do_tree = typename red::containers::splay_do_tree<int>;

TEST (Test_set, Test_select_1)
{
    splay_do_tree tree;

    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    for ( int i = 0; i < 5; i++ )
        tree.erase (i);

    EXPECT_EQ (*tree.os_select (1), 5);
    EXPECT_EQ (*tree.os_select (2), 6);
    EXPECT_EQ (*tree.os_select (3), 7);
    EXPECT_EQ (*tree.os_select (4), 8);
    EXPECT_EQ (*tree.begin (), 5);

    EXPECT_EQ (tree.os_select (0), tree.end ());
    EXPECT_EQ (tree.os_select (10), tree.end ());
}
TEST (Test_set, Test_select_2)
{
    splay_do_tree tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    for ( int i = 5; i <= 10; i++ )
        tree.erase (i);

    EXPECT_EQ (*tree.os_select (1), 1);
    EXPECT_EQ (*tree.os_select (2), 2);
    EXPECT_EQ (*tree.os_select (3), 3);
    EXPECT_EQ (*tree.os_select (4), 4);
    EXPECT_EQ (*std::prev (tree.end ()), 4);

    EXPECT_EQ (tree.os_select (0), tree.end ());
    EXPECT_EQ (tree.os_select (10), tree.end ());
}

TEST (Test_set, Test_number_less_then)
{
    splay_do_tree tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.erase (1);
    tree.erase (7);
    tree.erase (4);

    EXPECT_EQ (tree.get_number_less_then (11), 7);
    EXPECT_EQ (tree.get_number_less_then (4), 2);
}
