#include <gtest/gtest.h>

#include "splay_do_tree.hpp"

template struct red::containers::splay_do_tree<int>;
using splay_do_tree = typename red::containers::splay_do_tree<int>;

TEST (test_splay_do_tree, lower_bound)
{
    splay_do_tree tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.erase (1);
    tree.dump ("dump.dot");
    tree.erase (7);
    tree.erase (4);

    EXPECT_EQ (tree.lower_bound (11), tree.end ());
    EXPECT_EQ (*tree.lower_bound (2), 2);
    EXPECT_EQ (*tree.lower_bound (1), 2);
    EXPECT_EQ (*tree.lower_bound (-1), 2);
}

TEST (test_splay_do_tree, upper_bound)
{
    splay_do_tree tree;

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

TEST (test_splay_do_tree, iterator_decrement)
{
    splay_do_tree tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.erase (1);
    tree.erase (7);
    tree.erase (4);

    // tree.dump ("dump.dot");

    EXPECT_EQ (*(--tree.find (6)), 5);
    EXPECT_EQ (*(--tree.find (8)), 6);
    EXPECT_EQ (*(--tree.find (10)), 9);
    EXPECT_EQ (*(--tree.find (11)), 10);
    EXPECT_EQ (*(--tree.find (3)), 2);
    EXPECT_EQ ((--tree.find (2)).m_node, nullptr);
}

TEST (test_splay_do_tree, iterator_increment)
{
    splay_do_tree tree;

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
TEST (test_splay_do_tree, erase_the_last_one)
{
    splay_do_tree tree;
    tree.insert (1);
    tree.erase (tree.begin ());

    EXPECT_TRUE (tree.empty ());
    EXPECT_EQ (tree.begin (), tree.end ());
}

TEST (test_splay_do_tree, erase_leftmost)
{
    splay_do_tree tree;
    for ( int i = 6; i <= 10; i++ )
        tree.insert (i);
    EXPECT_EQ (*tree.begin (), 6);
    tree.erase (tree.begin ());
    EXPECT_EQ (*tree.begin (), 7);
}

TEST (test_splay_do_tree, erase_rightmost)
{
    splay_do_tree tree;
    for ( int i = 6; i <= 10; i++ )
        tree.insert (i);
    auto last = std::prev (tree.end ());
    EXPECT_EQ (*last, 10);

    tree.erase (last);

    last = std::prev (tree.end ());
    EXPECT_EQ (*last, 9);
}

TEST (test_splay_do_tree, erase_by_key_1)
{
    splay_do_tree tree;
    tree.insert (1);
    tree.erase (1);

    EXPECT_TRUE (tree.empty ());
}

TEST (test_splay_do_tree, erase_by_key_2)
{
    splay_do_tree tree;
    tree.insert (1);
    tree.insert (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 2);
    tree.erase (1);

    EXPECT_EQ (*tree.begin (), 2);
    EXPECT_EQ (*tree.rbegin (), 2);
}

TEST (test_splay_do_tree, erase_by_key_3)
{
    splay_do_tree tree;
    tree.insert (1);
    tree.insert (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 2);
    tree.erase (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 1);
}

TEST (test_splay_do_tree, delete_all)
{
    splay_do_tree tree;

    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    for ( int i = 0; i < 10; i++ )
        EXPECT_NO_THROW (tree.erase (i));

    EXPECT_EQ (tree.size (), 0);
    EXPECT_TRUE (tree.empty ());
}

TEST (test_splay_do_tree, test_distance)
{
    splay_do_tree tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    auto it1 = tree.find (3);
    auto it2 = tree.find (9);
    EXPECT_EQ (std::distance (it1, it2), 6);
}

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
