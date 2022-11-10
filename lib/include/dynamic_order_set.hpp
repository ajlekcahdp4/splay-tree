/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

#pragma once

#include "base_set.hpp"
#include "tree_node.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <memory>
#include <tuple>
#include <unordered_map>

namespace red
{
namespace containers
{
template <typename T, class Compare_t = std::less<T>>
struct dynamic_order_set : public base_set<T, Compare_t>
{
  protected:
    using base          = base_set<T, Compare_t>;
    using node          = dynamic_order_set_node<T>;
    using node_ptr      = node *;
    using base_node     = typename base::base_node;
    using base_node_ptr = typename base::base_node_ptr;

  public:
    using value_type = T;
    using size_type  = typename node::size_type;

    using typename base::iterator;
    using typename base::reverse_iterator;

    void insert (const value_type &key) override
    {
        base_node_ptr to_insert = new node {key};
        base::insert_base (
            to_insert, [] (base_node_ptr node) { static_cast<node_ptr> (node)->m_size++; },
            [] (base_node_ptr node) { static_cast<node_ptr> (node)->m_size--; });
    }

    void erase (const value_type &key) override
    {
        auto to_erase = base::find_for_erase (
            key, [] (base_node_ptr node) { static_cast<node_ptr> (node)->m_size--; },
            [] (base_node_ptr node) { static_cast<node_ptr> (node)->m_size++; });
        base::erase_base (to_erase);
    }

    void dump (std::ostream &stream) const override
    {
        assert (stream);
        stream << "digraph {\nrankdir = TB\n";
        for ( auto pos = base::begin (); pos != base::end (); pos++ )
        {
            stream << "\tnode" << pos.m_node << "[label = \"val: " << *pos
                   << " | size: " << static_cast<node_ptr> (pos.m_node)->m_size
                   << "\", shape=record, style=filled, fillcolor=palegreen];\n";

            if ( pos.m_node->m_left )
                stream << "\tnode" << pos.m_node << " -> node" << pos.m_node->m_left
                       << " [color=black, label=\"lchild\"];\n";
            else
            {
                stream << "\tnode" << pos.m_node << " -> node0_l_" << pos.m_node
                       << " [color=black, label=\"lchild\"];\n";
                stream << "\tnode0_l_" << pos.m_node
                       << " [label = \"\", shape=triangle, style=filled, fillcolor=black ];\n";
            }

            if ( pos.m_node->m_right )
                stream << "\tnode" << pos.m_node << " -> node" << pos.m_node->m_right
                       << " [color=black, label=\"rchild\"];\n";
            else
            {
                stream << "\tnode" << pos.m_node << " -> node0_r_" << pos.m_node
                       << " [color=black, label=\"rchild\"];\n";
                stream << "\tnode0_r_" << pos.m_node
                       << " [label = \"\", shape=triangle, style=filled, fillcolor=black];\n";
            }
            stream << "\tnode" << pos.m_node << " -> node" << pos.m_node->m_parent
                   << " [color=red, label=\" parent \"];\n";
        }
        stream << "}\n";
    }

    iterator os_select (size_type p_rank) const
    {
        if ( p_rank > base::size () || !p_rank )
            return iterator {nullptr, this};

        auto curr      = base::root ();
        size_type rank = (curr->m_left ? node::size (curr->m_left) : 0) + 1;
        while ( rank != p_rank )
        {
            if ( p_rank < rank )
                curr = curr->m_left;
            else
            {
                curr = curr->m_right;
                p_rank -= rank;
            }
            rank = (curr->m_left ? node::size (curr->m_left) : 0) + 1;
        }
        return iterator {static_cast<node_ptr> (curr), this};
    }

    size_type get_number_less_then (value_type val) const
    {
        if ( base::empty () )
            return 0;
        const auto &min_val = static_cast<node_ptr> (base::leftmost ())->m_value;
        if ( base::compare (val, min_val) || val == min_val )
            return 0;
        auto closest_left = --base::upper_bound (val);
        auto rank         = get_rank_of (closest_left.m_node);
        return (*closest_left == val ? rank - 1 : rank);
    }

  protected:
    virtual size_type get_rank_of (base_node_ptr node) const
    {
        auto [node_dummy, rank] = get_rank_of_base (node);
        return rank;
    }

    std::pair<base_node_ptr, size_type> get_rank_of_base (base_node_ptr node) const
    {
        size_type rank = (node->m_left ? node::size (node->m_left) + 1 : 1);
        while ( node != base::root () )
        {
            if ( !node->is_left_child () )
                rank += (node->m_parent->m_left ? node::size (node->m_parent->m_left) + 1 : 1);
            node = node->m_parent;
        }
        return {node, rank};
    }
};

}   // namespace containers
}   // namespace red