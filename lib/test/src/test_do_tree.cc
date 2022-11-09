#include <gtest/gtest.h>

#include "binary_tree.hpp"

template struct red::containers::binary_tree<red::containers::dynamic_set_node<int>>;
using binary_tree = typename red::containers::binary_tree<red::containers::dynamic_set_node<int>>;

TEST (test_binary_tree, ctor) { binary_tree {}; }

TEST (test_binary_tree, double_insert)
{
    binary_tree tree;
    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    auto old_size = tree.size ();

    for ( int i = 0; i < 10; i++ )
    {
        EXPECT_THROW (tree.insert (i), std::out_of_range);
        EXPECT_EQ (tree.size (), old_size);
    }
}

TEST (test_binary_tree, delete_unrepresented)
{
    binary_tree tree;
    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    auto old_size = tree.size ();

    for ( int i = -1; i < -10; i-- )
    {
        tree.insert (i);
        EXPECT_EQ (tree.size (), old_size);
    }
}

TEST (test_binary_tree, range_for)
{
    binary_tree tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    std::vector<int> v {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

    for ( auto &i : tree )
    {
        EXPECT_EQ (i, v.back ());
        v.pop_back ();
    }
}

TEST (test_binary_tree, clear)
{
    binary_tree tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.clear ();

    EXPECT_TRUE (tree.empty ());
    EXPECT_EQ (tree.begin (), tree.end ());
}

TEST (test_binary_tree, equal_1)
{
    binary_tree tree1;

    for ( int i = 1; i <= 10; i++ )
        tree1.insert (i);

    binary_tree tree2;

    for ( int i = 1; i <= 10; i++ )
        tree2.insert (2 * i);

    EXPECT_FALSE (tree1 == tree2);

    EXPECT_TRUE (tree1 != tree2);
}

TEST (test_binary_tree, move_ctor)
{
    binary_tree tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    std::vector<int> v {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    binary_tree tree2 = std::move (tree);
    for ( auto &i : tree2 )
    {
        EXPECT_EQ (i, v.back ());
        v.pop_back ();
    }
}

TEST (test_binary_tree, erase_by_key_1)
{
    binary_tree tree;
    tree.insert (1);
    tree.erase (1);

    EXPECT_TRUE (tree.empty ());
}

TEST (test_binary_tree, erase_by_key_2)
{
    binary_tree tree;
    tree.insert (1);
    tree.insert (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 2);
    tree.erase (1);

    EXPECT_EQ (*tree.begin (), 2);
    EXPECT_EQ (*tree.rbegin (), 2);
}

TEST (test_binary_tree, erase_by_key_3)
{
    binary_tree tree;
    tree.insert (1);
    tree.insert (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 2);
    tree.erase (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 1);
}

TEST (test_binary_tree, delete_all)
{
    binary_tree tree;

    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    for ( int i = 0; i < 10; i++ )
        EXPECT_NO_THROW (tree.erase (i));

    EXPECT_EQ (tree.size (), 0);
    EXPECT_TRUE (tree.empty ());
}

TEST (test_binary_tree, dump)
{
    std::ofstream os {"dump.txt"};
    binary_tree tree;
    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);
    tree.dump (os);
}