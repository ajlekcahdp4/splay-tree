#include <gtest/gtest.h>

#include "do_tree.hpp"

template struct red::containers::do_tree<int>;
using do_tree = typename red::containers::do_tree<int>;

TEST (test_do_tree, ctor) { do_tree {}; }
TEST (test_do_tree, iterator_decrement)
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

TEST (test_do_tree, iterator_increment)
{
    do_tree tree;

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
TEST (test_do_tree, erase_the_last_one)
{
    do_tree tree;
    tree.insert (1);
    tree.erase (tree.begin ());

    EXPECT_TRUE (tree.empty ());
    EXPECT_EQ (tree.begin (), tree.end ());
}

TEST (test_do_tree, erase_leftmost)
{
    do_tree tree;
    for ( int i = 6; i <= 10; i++ )
    {
        tree.insert (i);
        std::stringstream ss;
        ss << "dump" << i;
        tree.dump (ss.str ());
    }

    EXPECT_EQ (*tree.begin (), 6);
    tree.erase (tree.begin ());
    EXPECT_EQ (*tree.begin (), 7);
}

TEST (test_do_tree, erase_rightmost)
{
    do_tree tree;
    for ( int i = 6; i <= 10; i++ )
    {
        tree.insert (i);
        std::stringstream ss;
        ss << "dump" << i;
        tree.dump (ss.str ());
    }

    auto last = std::prev (tree.end ());
    EXPECT_EQ (*last, 10);

    tree.erase (last);

    last = std::prev (tree.end ());
    EXPECT_EQ (*last, 9);
}

TEST (test_do_tree, erase_by_key_1)
{
    do_tree tree;
    tree.insert (1);
    tree.erase (1);

    EXPECT_TRUE (tree.empty ());
}

TEST (test_do_tree, erase_by_key_2)
{
    do_tree tree;
    tree.insert (1);
    tree.insert (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 2);
    tree.erase (1);

    EXPECT_EQ (*tree.begin (), 2);
    EXPECT_EQ (*tree.rbegin (), 2);
}

TEST (test_do_tree, erase_by_key_3)
{
    do_tree tree;
    tree.insert (1);
    tree.insert (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 2);
    tree.erase (2);

    EXPECT_EQ (*tree.begin (), 1);
    EXPECT_EQ (*tree.rbegin (), 1);
}

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

TEST (test_do_tree, delete_all)
{
    do_tree tree;

    for ( int i = 0; i < 10; i++ )
        tree.insert (i);

    for ( int i = 0; i < 10; i++ )
        EXPECT_NO_THROW (tree.erase (i));

    EXPECT_EQ (tree.size (), 0);
    EXPECT_TRUE (tree.empty ());
}

TEST (test_do_tree, lower_bound)
{
    do_tree tree;

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

TEST (test_do_tree, upper_bound)
{
    do_tree tree;

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

TEST (test_do_tree, range_for)
{
    do_tree tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.erase (1);
    tree.erase (7);
    tree.erase (4);

    std::vector<int> v {10, 9, 8, 6, 5, 3, 2};

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

    tree.erase (1);
    tree.erase (7);
    tree.erase (4);

    tree.clear ();

    EXPECT_TRUE (tree.empty ());
    EXPECT_EQ (tree.begin (), tree.end ());
}

TEST (test_do_tree, equal_1)
{
    do_tree tree1;

    for ( int i = 1; i <= 10; i++ )
        tree1.insert (i);

    tree1.erase (1);
    tree1.erase (7);
    tree1.erase (4);

    do_tree tree2;

    for ( int i = 1; i <= 10; i++ )
        tree2.insert (i);

    tree2.erase (1);
    tree2.erase (4);

    do_tree tree3;

    for ( int i = 1; i <= 10; i++ )
        tree3.insert (i);

    tree3.erase (1);
    tree3.erase (7);
    tree3.erase (5);

    EXPECT_FALSE (tree1 == tree2);
    EXPECT_FALSE (tree1 == tree3);
    EXPECT_FALSE (tree2 == tree3);

    EXPECT_TRUE (tree1 != tree2);
    EXPECT_TRUE (tree1 != tree3);
    EXPECT_TRUE (tree2 != tree3);
}

TEST (test_do_tree, equal_2)
{
    do_tree tree1;

    for ( int i = 1; i <= 10; i++ )
        tree1.insert (i);

    tree1.erase (1);
    tree1.erase (7);
    tree1.erase (4);

    do_tree tree2;

    for ( int i = 1; i <= 10; i++ )
        tree2.insert (i);

    tree2.erase (1);
    tree2.erase (7);
    tree2.erase (4);

    EXPECT_TRUE (tree1 == tree2);
    EXPECT_FALSE (tree1 != tree2);
}

TEST (test_do_tree, move_ctor)
{
    do_tree tree;

    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);

    tree.erase (1);
    tree.erase (7);
    tree.erase (4);

    std::vector<int> v {10, 9, 8, 6, 5, 3, 2};
    do_tree tree2 = std::move (tree);
    for ( auto &i : tree2 )
    {
        EXPECT_EQ (i, v.back ());
        v.pop_back ();
    }
}