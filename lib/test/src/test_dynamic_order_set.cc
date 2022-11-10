#include <gtest/gtest.h>
#include <set>

#include "dynamic_order_set.hpp"

template struct red::containers::dynamic_order_set<int>;
using base_set = typename red::containers::dynamic_order_set<int>;

TEST (set_dynamic_order_set, ctor) { base_set {}; }

TEST (set_dynamic_order_set, dump)
{
    std::ofstream os {"dump_dynamic_order_set.txt"};
    base_set tree;
    for ( int i = 1; i <= 10; i++ )
        tree.insert (i);
    tree.dump (os);
}