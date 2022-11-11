#include <gtest/gtest.h>
#include <set>

#include "splay_dynamic_order_set.hpp"

template struct red::containers::splay_dynamic_order_set<int>;
using splay_set = typename red::containers::splay_dynamic_order_set<int>;

TEST (test_splay_set, ctor) { splay_set {}; }

TEST (test_splay_set, double_insert)
{
    splay_set tree;
    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    auto old_size = tree.size ();

    for ( int i = 0; i < 10; i++ )
    {
        EXPECT_THROW (tree.insert (i), std::out_of_range);
        EXPECT_EQ (tree.size (), old_size);
    }
}

TEST (test_splay_set, delete_unrepresented)
{
    splay_set tree;
    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    auto old_size = tree.size ();

    for ( int i = -1; i < -10; i-- )
    {
        tree.insert (i);
        EXPECT_EQ (tree.size (), old_size);
    }
}

TEST (test_splay_set, range_for)
{
    splay_set tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    std::vector<int> v {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

    for ( auto &i : tree )
    {
        EXPECT_EQ (i, v.back ());
        v.pop_back ();
    }
}

TEST (test_splay_set, clear)
{
    splay_set tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.clear ();

    EXPECT_TRUE (tree.empty ());
    EXPECT_EQ (tree.begin (), tree.end ());
}

TEST (test_splay_set, equal_1)
{
    splay_set tree1;

    for ( int i = 1; i <= 10; i++ )
        tree1.insert (i);

    splay_set tree2;

    for ( int i = 1; i <= 10; i++ )
        tree2.insert (2 * i);

    EXPECT_FALSE (tree1 == tree2);

    EXPECT_TRUE (tree1 != tree2);
}

TEST (test_splay_set, move_ctor)
{
    splay_set tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    std::vector<int> v {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    splay_set tree2 = std::move (tree);
    for ( auto &i : tree2 )
    {
        EXPECT_EQ (i, v.back ());
        v.pop_back ();
    }
}

TEST (test_splay_set, erase_leftmost)
{
    splay_set tree;
    for ( int i = 6; i <= 10; i++ )
        tree.insert (i);
    EXPECT_EQ (*tree.begin (), 6);
    tree.erase (tree.begin ());
    EXPECT_EQ (*tree.begin (), 7);
}

TEST (test_splay_set, erase_rightmost)
{
    splay_set tree;
    for ( int i = 6; i <= 10; i++ )
        tree.insert (i);
    auto last = std::prev (tree.end ());
    EXPECT_EQ (*last, 10);

    tree.erase (last);

    last = std::prev (tree.end ());
    EXPECT_EQ (*last, 9);
}

TEST (test_splay_set, erase_by_key_1)
{
    splay_set tree;
    tree.insert (1);
    tree.erase (1);

    EXPECT_TRUE (tree.empty ());
}

TEST (test_splay_set, erase_by_key_2)
{
    splay_set tree;
    tree.insert (1);
    tree.insert (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 2);
    tree.erase (1);

    EXPECT_EQ (*tree.begin (), 2);
    EXPECT_EQ (*tree.rbegin (), 2);
}

TEST (test_splay_set, erase_by_key_3)
{
    splay_set tree;
    tree.insert (1);
    tree.insert (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 2);
    tree.erase (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 1);
}

TEST (test_splay_set, erase_by_key_4)
{
    splay_set tree;
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

TEST (test_splay_set, erase_by_key_5)
{
    splay_set tree;
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

TEST (test_splay_set, delete_all)
{
    splay_set tree;

    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    for ( int i = 0; i < 10; i++ )
        EXPECT_NO_THROW (tree.erase (i));

    EXPECT_EQ (tree.size (), 0);
    EXPECT_TRUE (tree.empty ());
}

TEST (test_splay_set, test_loop_erase)
{
    splay_set tree;
    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    for ( int i = 5; i <= 10; i++ )
        tree.erase (i);
}

TEST (test_splay_set, find)
{
    splay_set tree;
    std::set<int> std_set {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for ( auto &el : std_set )
        tree.insert (el);
    tree.erase (2);
    tree.erase (4);
    tree.erase (7);
    std_set.erase (2);
    std_set.erase (4);
    std_set.erase (7);
    auto it1 = tree.find (5);
    auto it2 = std_set.find (5);
    EXPECT_EQ (*it1, *it2);
}

TEST (test_splay_set, lower_bound)
{
    splay_set tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.erase (1);
    tree.erase (7);
    tree.erase (4);

    EXPECT_EQ (tree.lower_bound (11), tree.end ());
    EXPECT_EQ (*tree.lower_bound (2), 2);
    EXPECT_EQ (*tree.lower_bound (1), 2);
    EXPECT_EQ (*tree.lower_bound (-1), 2);
}

TEST (test_splay_set, upper_bound)
{
    splay_set tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.erase (1);
    tree.erase (7);
    tree.erase (4);

    EXPECT_EQ (tree.upper_bound (11), tree.end ());
    EXPECT_EQ (*tree.upper_bound (2), 3);
    EXPECT_EQ (*tree.upper_bound (1), 2);
    EXPECT_EQ (*tree.upper_bound (-1), 2);
}

TEST (test_splay_set, iterator_decrement)
{
    splay_set tree;

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

TEST (test_splay_set, iterator_increment)
{
    splay_set tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.erase (1);
    tree.erase (7);
    tree.erase (4);

    EXPECT_EQ (*(++tree.find (6)), 8);
    EXPECT_EQ (*(++tree.find (8)), 9);
    EXPECT_EQ ((++tree.find (10)), tree.end ());
    EXPECT_EQ (*(++tree.find (3)), 5);
}

TEST (test_splay_set, test_distance)
{
    splay_set tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    auto it1 = tree.find (3);
    auto it2 = tree.find (9);
    EXPECT_EQ (std::distance (it1, it2), 6);
}

TEST (test_splay_set, copy_ctor)
{
    splay_set tree1;

    for ( int i = 1; i <= 10; i++ )
        tree1.insert (i);

    tree1.erase (1);
    tree1.erase (7);
    tree1.erase (4);

    splay_set tree2 {tree1};
    std::set<int> std_set (tree1.begin (), tree1.end ());

    EXPECT_EQ (tree1, tree2);
    tree1.clear ();
    EXPECT_TRUE (std::equal (tree2.begin (), tree2.end (), std_set.begin ()));
}

TEST (test_splay_set, copy_assignment)
{
    splay_set tree1;
    for ( int i = 1; i <= 10; i++ )
        tree1.insert (i);
    tree1.erase (1);
    tree1.erase (7);
    tree1.erase (4);

    splay_set tree2;
    for ( int i = 10; i <= 20; i++ )
        tree2.insert (i);
    tree2.erase (11);
    tree2.erase (17);
    tree2.erase (14);

    EXPECT_NE (tree1, tree2);

    tree2 = tree1;
    EXPECT_EQ (tree1, tree2);
}

TEST (test_splay_set, dump)
{
    std::ofstream os {"dump_splay_set.txt"};
    splay_set tree;
    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);
    tree.dump (os);
}