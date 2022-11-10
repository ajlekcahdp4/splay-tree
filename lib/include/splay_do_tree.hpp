/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

// splay tree wrapper for do_tree

#pragma once

#include "binary_tree.hpp"
#include "tree_node.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <memory>
#include <tuple>
#include <unordered_map>

// bool operator== (const self &other) const
// {
//     return size () == other.size () && std::equal (begin (), end (), other.begin ());
// }

// bool operator!= (const self &other) const { return !(*this == other); }

// std::swap (static_cast<node_ptr> (target)->m_value,
//                    static_cast<node_ptr> (to_erase)->m_value);

namespace red
{
namespace containers
{
template <typename T, class Compare_t = std::less<T>>
struct splay_do_tree : public binary_tree<T, Compare_t>
{
  private:
    using base_tree = binary_tree<Compare_t>;
    using node      = dynamic_set_node<T>;
    using node_ptr  = node *;
    using typename base_tree::base_node;
    using typename base_tree::base_node_ptr;

  public:
    using compare    = Compare_t;
    using value_type = T;
    using size_type  = node::size_type;

    // modifiers

    void clear () { m_header_struct.m_reset (); }
};

}   // namespace containers
}   // namespace red