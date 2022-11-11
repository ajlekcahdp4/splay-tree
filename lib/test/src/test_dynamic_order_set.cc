#include <gtest/gtest.h>
#include <set>

#include "dynamic_order_set.hpp"

template struct red::containers::dynamic_order_set<int>;
using do_set = typename red::containers::dynamic_order_set<int>;

TEST (test_dynamic_order_set, ctor) { do_set {}; }

TEST (test_dynamic_order_set, select_1)
{
    do_set tree;

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
TEST (test_dynamic_order_set, select_2)
{
    do_set tree;

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

TEST (test_dynamic_order_set, get_number_less_then)
{
    do_set tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.erase (1);
    tree.erase (7);
    tree.erase (4);

    EXPECT_EQ (tree.get_number_less_then (11), 7);
    EXPECT_EQ (tree.get_number_less_then (4), 2);
}

TEST (test_dynamic_order_set, copy_ctor)
{
    do_set tree1;

    for ( int i = 1; i <= 10; i++ )
        tree1.insert (i);

    tree1.erase (1);
    tree1.erase (7);
    tree1.erase (4);

    do_set tree2 {tree1};
    std::set<int> std_set (tree1.begin (), tree1.end ());

    EXPECT_EQ (tree1, tree2);
    tree1.clear ();
    EXPECT_TRUE (std::equal (tree2.begin (), tree2.end (), std_set.begin ()));
}

TEST (test_dynamic_order_set, copy_assignment)
{
    do_set tree1;
    for ( int i = 1; i <= 10; i++ )
        tree1.insert (i);
    tree1.erase (1);
    tree1.erase (7);
    tree1.erase (4);

    do_set tree2;
    for ( int i = 10; i <= 20; i++ )
        tree2.insert (i);
    tree2.erase (11);
    tree2.erase (17);
    tree2.erase (14);

    EXPECT_NE (tree1, tree2);

    tree2 = tree1;
    EXPECT_EQ (tree1, tree2);
}

TEST (test_dynamic_order_set, dump)
{
    std::ofstream os {"dump_dynamic_order_set.txt"};
    do_set tree;
    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);
    tree.dump (os);
}