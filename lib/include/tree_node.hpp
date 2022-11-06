/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

// node structs for binary tree

#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <tuple>
#include <utility>

namespace red
{
namespace containers
{

struct dl_binary_tree_node_base
{
  private:
    dl_binary_tree_node_base *m_parent = nullptr;
    dl_binary_tree_node_base *m_left   = nullptr;
    dl_binary_tree_node_base *m_right  = nullptr;

  public:
    virtual ~dl_binary_tree_node_base () {};

    dl_binary_tree_node_base *minimum ()
    {
        auto node = this;
        while ( node->m_left )
            node = node->m_left;
        return node;
    };

    dl_binary_tree_node_base *maximum ()
    {
        auto node = this;
        while ( node->m_right )
            node = node->m_right;
        return node;
    };

    dl_binary_tree_node_base *successor () const;
    dl_binary_tree_node_base *predecessor () const;

    bool is_left_child () const { return (m_parent ? this == m_parent->m_left : false); }
    bool is_linear () const { return m_parent && (is_left_child () == m_parent->is_left_child ()); }

    dl_binary_tree_node_base *rotate_left_base ();
    dl_binary_tree_node_base *rotate_right_base ();
    virtual dl_binary_tree_node_base *rotate_left () { return rotate_left_base (); }
    virtual dl_binary_tree_node_base *rotate_right () { return rotate_right_base (); }
    dl_binary_tree_node_base *rotate_to_parent ()
    {
        if ( is_left_child () )
            return m_parent->rotate_right ();
        else
            return m_parent->rotate_left ();
    }
};

struct do_tree_node_base
{
    using size_type     = std::size_t;
    using base_node_ptr = do_tree_node_base *;
    using self          = do_tree_node_base;
    using owning_ptr    = typename std::unique_ptr<do_tree_node_base>;

    size_type m_size = 1;

    static size_type size (dl_binary_tree_node_base *node) { return (node ? node->m_size : 0); }

    dl_binary_tree_node_base *m_minimum ()
    {
        dl_binary_tree_node_base *x = this;
        while ( x->m_left.get () )
            x = x->m_left.get ();
        return x;
    }

    dl_binary_tree_node_base *m_maximum ()
    {
        dl_binary_tree_node_base *x = this;
        while ( x->m_right.get () )
            x = x->m_right.get ();
        return x;
    }

    dl_binary_tree_node_base *do_tree_increment () const;
    dl_binary_tree_node_base *do_tree_decrement () const;

    dl_binary_tree_node_base *rotate_left ();
    dl_binary_tree_node_base *rotate_right ();
    dl_binary_tree_node_base *rotate_to_parent ()
    {
        if ( is_left_child () )
            return m_parent->rotate_right ();
        else
            return m_parent->rotate_left ();
    }

    bool is_left_child () const { return (m_parent ? this == m_parent->m_left.get () : false); }

    bool is_linear () const { return m_parent && (is_left_child () == m_parent->is_left_child ()); }
};

}   // namespace containers
}   // namespace red
