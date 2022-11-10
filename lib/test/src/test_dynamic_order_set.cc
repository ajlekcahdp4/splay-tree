#include <gtest/gtest.h>
#include <set>

#include "dynamic_order_set.hpp"

template struct red::containers::dynamic_order_set<int>;
using base_set = typename red::containers::dynamic_order_set<int>;

TEST (set_dynamic_order_set, ctor) { base_set {}; }

TEST (set_dynamic_order_set, double_insert)
{
    base_set tree;
    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    auto old_size = tree.size ();

    for ( int i = 0; i < 10; i++ )
    {
        EXPECT_THROW (tree.insert (i), std::out_of_range);
        EXPECT_EQ (tree.size (), old_size);
    }
}

TEST (set_dynamic_order_set, delete_unrepresented)
{
    base_set tree;
    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    auto old_size = tree.size ();

    for ( int i = -1; i < -10; i-- )
    {
        tree.insert (i);
        EXPECT_EQ (tree.size (), old_size);
    }
}

TEST (set_dynamic_order_set, range_for)
{
    base_set tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    std::vector<int> v {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

    for ( auto &i : tree )
    {
        EXPECT_EQ (i, v.back ());
        v.pop_back ();
    }
}

TEST (set_dynamic_order_set, clear)
{
    base_set tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.clear ();

    EXPECT_TRUE (tree.empty ());
    EXPECT_EQ (tree.begin (), tree.end ());
}

TEST (set_dynamic_order_set, equal_1)
{
    base_set tree1;

    for ( int i = 1; i <= 10; i++ )
        tree1.insert (i);

    base_set tree2;

    for ( int i = 1; i <= 10; i++ )
        tree2.insert (2 * i);

    EXPECT_FALSE (tree1 == tree2);

    EXPECT_TRUE (tree1 != tree2);
}

TEST (set_dynamic_order_set, move_ctor)
{
    base_set tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    std::vector<int> v {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    base_set tree2 = std::move (tree);
    for ( auto &i : tree2 )
    {
        EXPECT_EQ (i, v.back ());
        v.pop_back ();
    }
}

TEST (set_dynamic_order_set, erase_by_key_1)
{
    base_set tree;
    tree.insert (1);
    tree.erase (1);

    EXPECT_TRUE (tree.empty ());
}

TEST (set_dynamic_order_set, erase_by_key_2)
{
    base_set tree;
    tree.insert (1);
    tree.insert (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 2);
    tree.erase (1);

    EXPECT_EQ (*tree.begin (), 2);
    EXPECT_EQ (*tree.rbegin (), 2);
}

TEST (set_dynamic_order_set, erase_by_key_3)
{
    base_set tree;
    tree.insert (1);
    tree.insert (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 2);
    tree.erase (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 1);
}

TEST (set_dynamic_order_set, erase_by_key_4)
{
    base_set tree;
    std::set<int> std_set {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for ( auto &el : std_set )
        tree.insert (el);
    EXPECT_TRUE (std::equal (tree.begin (), tree.end (), std_set.begin ()));
    tree.erase (2);
    tree.erase (4);
    tree.erase (7);
    std_set.erase (2);
    std_set.erase (4);
    std_set.erase (7);
    auto it1 = tree.begin ();
    for ( auto it2 = std_set.begin (); it2 != std_set.end (); ++it1, ++it2 )
        EXPECT_EQ (*it1, *it2);
}

TEST (set_dynamic_order_set, erase_by_key_5)
{
    base_set tree;
    std::set<int> std_set {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for ( auto &el : std_set )
        tree.insert (el);
    EXPECT_TRUE (std::equal (tree.begin (), tree.end (), std_set.begin ()));
    tree.erase (1);
    tree.erase (4);
    tree.erase (10);
    std_set.erase (1);
    std_set.erase (4);
    std_set.erase (10);
    EXPECT_TRUE (std::equal (tree.begin (), tree.end (), std_set.begin ()));
}

TEST (set_dynamic_order_set, delete_all)
{
    base_set tree;

    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    for ( int i = 0; i < 10; i++ )
        EXPECT_NO_THROW (tree.erase (i));

    EXPECT_EQ (tree.size (), 0);
    EXPECT_TRUE (tree.empty ());
}

TEST (set_dynamic_order_set, dump)
{
    std::ofstream os {"dump.txt"};
    base_set tree;
    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);
    tree.dump (os);
}