#include <gtest/gtest.h>

#include "do_tree.hpp"

template struct red::containers::do_tree<int>;
using do_tree = typename red::containers::do_tree<int>;

TEST (test_do_tree, test_ctor) { do_tree a; }