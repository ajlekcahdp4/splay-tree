#include <gtest/gtest.h>
#include <set>

#include "splay_dynamic_order_set.hpp"

template struct red::containers::splay_dynamic_order_set<int>;
using splay_set = typename red::containers::splay_dynamic_order_set<int>;

TEST (test_splay_set, ctor) { splay_set {}; }