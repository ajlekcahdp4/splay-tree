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
  public:
    using base_node     = dl_binary_tree_node_base;
    using base_node_ptr = dl_binary_tree_node_base *;

    dl_binary_tree_node_base *m_parent = nullptr;
    dl_binary_tree_node_base *m_left   = nullptr;
    dl_binary_tree_node_base *m_right  = nullptr;

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

  protected:
    dl_binary_tree_node_base *rotate_left_base ();
    dl_binary_tree_node_base *rotate_right_base ();

  public:
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

template <typename T> struct dynamic_set_node : public dl_binary_tree_node_base
{
    using value_type = T;
    using size_type  = typename std::size_t;
    using typename dl_binary_tree_node_base::base_node;
    using typename dl_binary_tree_node_base::base_node_ptr;

    size_type m_size = 1;
    value_type m_value {};

    static size_type size (const base_node_ptr base_ptr)
    {
        return static_cast<const dynamic_set_node *> (base_ptr)->m_size;
    }

    static value_type &value (const base_node_ptr base_ptr)
    {
        return static_cast<const dynamic_set_node *> (base_ptr)->m_value;
    }

    base_node_ptr rotate_left () override
    {
        auto *rchild               = static_cast<dynamic_set_node *> (rotate_left_base ());
        rchild->m_size             = m_size;
        m_size                     = 1 + size (m_left) + size (m_right);
        return rchild;
    }

    base_node_ptr rotate_right () override
    {
        auto *lchild               = static_cast<dynamic_set_node *> (rotate_right_base ());
        lchild->m_size             = m_size;
        m_size                     = 1 + size (m_left) + size (m_right);
        return lchild;
    }

    dynamic_set_node (T val) : m_value {val} {}
};

}   // namespace containers
}   // namespace red
