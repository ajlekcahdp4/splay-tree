#include <gtest/gtest.h>

#include "do_tree.hpp"

template struct red::containers::do_tree<int>;
using do_tree = typename red::containers::do_tree<int>;

TEST (test_do_tree, ctor) { do_tree {}; }

TEST (test_do_tree, double_insert)
{
    do_tree tree;
    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    auto old_size = tree.size ();

    for ( int i = 0; i < 10; i++ )
    {
        EXPECT_THROW (tree.insert (i), std::out_of_range);
        EXPECT_EQ (tree.size (), old_size);
    }
}

TEST (test_do_tree, delete_unrepresented)
{
    do_tree tree;
    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    auto old_size = tree.size ();

    for ( int i = -1; i < -10; i-- )
    {
        tree.insert (i);
        EXPECT_EQ (tree.size (), old_size);
    }
}

TEST (test_do_tree, range_for)
{
    do_tree tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    std::vector<int> v {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

    for ( auto &i : tree )
    {
        EXPECT_EQ (i, v.back ());
        v.pop_back ();
    }
}

TEST (test_do_tree, clear)
{
    do_tree tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.clear ();

    EXPECT_TRUE (tree.empty ());
    EXPECT_EQ (tree.begin (), tree.end ());
}

TEST (test_do_tree, equal_1)
{
    do_tree tree1;

    for ( int i = 1; i <= 10; i++ )
        tree1.insert (i);

    do_tree tree2;

    for ( int i = 1; i <= 10; i++ )
        tree2.insert (2 * i);

    EXPECT_FALSE (tree1 == tree2);

    EXPECT_TRUE (tree1 != tree2);
}

TEST (test_do_tree, move_ctor)
{
    do_tree tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    std::vector<int> v {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    do_tree tree2 = std::move (tree);
    for ( auto &i : tree2 )
    {
        EXPECT_EQ (i, v.back ());
        v.pop_back ();
    }
}