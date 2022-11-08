/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

// basic binary tree

#pragma once

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

// helper type offering value initialization guarantee on the compare functor.
template <class Compare> struct key_compare
{
    Compare m_value_compare;

    key_compare (const Compare &comp) : m_value_compare (comp) {}

    template <typename Key> bool operator() (const Key &k1, const Key &k2) const
    {
        return m_value_compare (k1, k2);
    }
};

// helper type to manage deafault initialization of node count, header and boundary nodes.
// stores all tree's nodes as unique pointers.
struct header
{
    using base_node     = dl_binary_tree_node_base;
    using base_node_ptr = dl_binary_tree_node_base *;

    base_node_ptr m_header    = nullptr;
    base_node_ptr m_leftmost  = nullptr;
    base_node_ptr m_rightmost = nullptr;
    std::unordered_map<base_node_ptr, std::unique_ptr<base_node>> m_nodes;
    std::size_t m_size {0};

    header () : m_header {new base_node} { m_nodes.emplace (m_header, m_header); }

    void m_reset ()
    {
        m_leftmost  = nullptr;
        m_rightmost = nullptr;
        m_header    = nullptr;
        m_size      = 0;
        m_nodes.clear ();
        m_header = new base_node;
        m_nodes.emplace (m_header, m_header);
    }
};

template <typename node_t, class Compare_t = std::less<typename node_t::value_type>>
struct binary_tree
{
  private:
    using key_compare_t = key_compare<Compare_t>;
    using self          = binary_tree<node_t, Compare_t>;

    using node          = node_t;
    using node_ptr      = node_t *;
    using base_node     = typename node::base_node;
    using base_node_ptr = typename node::base_node_ptr;

    using compare = Compare_t;

    struct binary_tree_iterator
    {
        using iterator_category = typename std::bidirectional_iterator_tag;
        using difference_type   = typename std::ptrdiff_t;
        using value_type        = typename node_t::value_type;
        using pointer           = value_type *;
        using reference         = value_type &;

        reference operator* () const { return m_node->m_value; }

        pointer get () { return &m_node->m_value; }

        pointer operator->() { return get (); }

        binary_tree_iterator &operator++ ()
        {
            m_node = static_cast<node_ptr> (m_node->successor ());
            return *this;
        }

        binary_tree_iterator operator++ (int)
        {
            auto tmp = *this;
            m_node   = static_cast<node_ptr> (m_node->successor ());
            return tmp;
        }

        binary_tree_iterator &operator-- ()
        {
            m_node = (m_node ? static_cast<node_ptr> (m_node->predecessor ())
                             : static_cast<node_ptr> (m_tree->m_header_struct.m_rightmost));
            return *this;
        }

        binary_tree_iterator operator-- (int)
        {
            auto tmp = *this;
            m_node   = (m_node ? static_cast<node_ptr> (m_node->predecessor ())
                               : static_cast<node_ptr> (m_tree->m_header_struct.m_rightmost));
            return tmp;
        }

        bool operator== (const binary_tree_iterator &other) const { return m_node == other.m_node; }

        bool operator!= (const binary_tree_iterator &other) const { return !(*this == other); }

        node_ptr m_node                          = nullptr;
        const binary_tree<node, compare> *m_tree = nullptr;
    };

  public:
    using value_type = typename node_t::value_type;
    using pointer    = value_type *;
    using reference  = value_type &;
    using size_type  = typename std::size_t;

  private:
    key_compare_t m_compare_struct;
    header m_header_struct;

  public:
    using iterator         = binary_tree_iterator;
    using reverse_iterator = typename std::reverse_iterator<iterator>;

  public:
    binary_tree () : m_compare_struct {Compare_t {}}, m_header_struct {} {}
    binary_tree (const Compare_t &comp) : m_compare_struct {comp}, m_header_struct {} {}

    binary_tree (const self &other)     = delete;
    self &operator= (const self &other) = delete;

    binary_tree (self &&rhs) noexcept : m_compare_struct {Compare_t {}}
    {
        std::swap (m_compare_struct, rhs.m_compare_struct);
        std::swap (m_header_struct, rhs.m_header_struct);
    }

    self &operator= (self &&rhs) noexcept
    {
        std::swap (m_compare_struct, rhs.m_compare_struct);
        std::swap (m_header_struct, rhs.m_header_struct);
        return *this;
    }

    virtual ~binary_tree () {}

    //  accessors
    iterator begin () const
    {
        return iterator {static_cast<node_ptr> (m_header_struct.m_leftmost), this};
    }

    iterator end () const { return iterator {nullptr, this}; }

    reverse_iterator rbegin () const { return reverse_iterator {end ()}; }

    reverse_iterator rend () const { return reverse_iterator {begin ()}; }

    size_type size () const { return m_header_struct.m_size; }

    bool empty () const { return !size (); }

    // modifiers

    void clear () { m_header_struct.m_reset (); }

    virtual iterator insert (const value_type &key)
    {
        auto to_insert = new node (key);
        m_header_struct.m_nodes.emplace (to_insert, to_insert);
        auto res = insert_node_base (
            to_insert, [] (base_node_ptr) {}, [] (base_node_ptr) {});
        ++m_header_struct.m_size;
        return iterator {static_cast<node_ptr> (res), this};
    }

    virtual void erase (const value_type &key)
    {
        auto to_erase_pos = find_for_erase (
            key, [] (base_node_ptr) {}, [] (base_node_ptr) {});

        erase_node_base (to_erase_pos.m_node);

        m_header_struct.m_nodes.erase (to_erase_pos.m_node);
        --m_header_struct.m_size;
    }

  protected:
    base_node_ptr root () const { return m_header_struct.m_header->m_left; }

    // insert node in tree without rebalancing.
    // "step" applies to all nodes on the path to the requested key
    // "step_if_already" applies to all nodes on the path to the requested key if this key has
    // already been inserted
    template <typename F1, typename F2>
    base_node_ptr insert_node_base (base_node_ptr to_insert, F1 step, F2 step_if_already);

    // find node to be erased from the tree
    // "step" applies to all nodes on the path to the requested key
    // "step_if_no" applies to all nodes on the path to the requested key if there were no
    // element with the requested key
    template <typename F1, typename F2>
    iterator find_for_erase (const value_type &key, F1 step, F2 step_if_no)
    {
        auto [found, prev, prev_greater] = trav_bin_search (key, step);
        if ( found )
            return iterator {static_cast<node_ptr> (found), this};
        trav_bin_search (key, step_if_no);
        throw std::out_of_range ("No element with requested key to erase");
    }

    void erase_node_base (base_node_ptr to_erase)
    {
        auto succ            = to_erase->successor ();
        base_node_ptr target = succ;
        if ( m_header_struct.m_leftmost == to_erase )
        {
            m_header_struct.m_leftmost = succ;
            target                     = to_erase;
        }
        if ( m_header_struct.m_rightmost == to_erase )
        {
            m_header_struct.m_rightmost = to_erase->predecessor ();
            target                      = to_erase;
        }
        std::swap (static_cast<node_ptr> (target)->m_value,
                   static_cast<node_ptr> (to_erase)->m_value);
    }

    template <typename F>
    std::tuple<base_node_ptr, base_node_ptr, bool> trav_bin_search (value_type key, F step) const;

  public:
    bool operator== (const self &other) const
    {
        return size () == other.size () && std::equal (begin (), end (), other.begin ());
    }

    bool operator!= (const self &other) const { return !(*this == other); }

    void dump (std::string filename) const
    {
        std::ofstream p_stream {filename};
        assert (p_stream);
        p_stream << "digraph {\nrankdir = TB\n";
        for ( auto pos = begin (); pos != end (); pos++ )
        {
            p_stream << "\tnode" << pos.m_node << "[label = \"{" << *pos << "} | "
                     << pos.m_node->m_size
                     << "\", shape=record, style=filled, fillcolor=palegreen];\n";

            if ( pos.m_node->m_left )
                p_stream << "\tnode" << pos.m_node << " -> node" << pos.m_node->m_left << ";\n";
            else
            {
                p_stream << "\tnode" << pos.m_node << " -> node0_l_" << *pos << ";\n";
                p_stream << "\tnode0_l_" << *pos
                         << " [label = \"\", shape=triangle, style=filled, fillcolor=black ];\n";
            }

            if ( pos.m_node->m_right )
                p_stream << "\tnode" << pos.m_node << " -> node" << pos.m_node->m_right << ";\n";
            else
            {
                p_stream << "\tnode" << pos.m_node << " -> node0_r_" << *pos << ";\n";
                p_stream << "\tnode0_r_" << *pos
                         << " [label = \"\", shape=triangle, style=filled, fillcolor=black];\n";
            }
            p_stream << "\tnode" << pos.m_node << " -> node" << pos.m_node->m_parent
                     << " [color=red];\n";
        }
        p_stream << "}\n";
    }
};

template <typename Node_t, typename Comp_t>
template <typename F>
std::tuple<typename binary_tree<Node_t, Comp_t>::base_node_ptr,
           typename binary_tree<Node_t, Comp_t>::base_node_ptr, bool>
binary_tree<Node_t, Comp_t>::trav_bin_search (value_type key, F step) const
{
    using res = typename std::tuple<base_node_ptr, base_node_ptr, bool>;

    auto prev = root ();
    auto curr = prev;
    if ( !curr )
        return res {nullptr, nullptr, false};

    bool key_less {};

    while ( curr && static_cast<node_ptr> (curr)->m_value != key )
    {
        key_less =
            self::m_compare_struct.m_value_compare (key, static_cast<node_ptr> (curr)->m_value);
        step (curr);
        prev = curr;
        if ( key_less )
            curr = curr->m_left;
        else
            curr = curr->m_right;
    }

    if ( curr == root () )
        step (curr);

    return res {curr, prev, key_less};
}

template <typename Node_t, typename Comp_t>
template <typename F1, typename F2>
typename binary_tree<Node_t, Comp_t>::base_node_ptr
binary_tree<Node_t, Comp_t>::insert_node_base (base_node_ptr to_insert, F1 step, F2 step_if_already)
{
    auto to_insert_ptr = to_insert;
    if ( empty () )
    {
        m_header_struct.m_header->m_left = std::move (to_insert);
        to_insert_ptr->m_parent          = m_header_struct.m_header;

        m_header_struct.m_leftmost  = to_insert_ptr;
        m_header_struct.m_rightmost = to_insert_ptr;

        return to_insert_ptr;
    }

    /* Find right position in the tree */
    auto [found, prev, prev_greater] =
        trav_bin_search (static_cast<node_ptr> (to_insert_ptr)->m_value, step);

    if ( found )
    {
        trav_bin_search (static_cast<node_ptr> (to_insert_ptr)->m_value, step_if_already);
        throw std::out_of_range ("Element already inserted");
    }
    to_insert->m_parent = prev;

    if ( prev == m_header_struct.m_header || prev_greater )
    {
        prev->m_left = std::move (to_insert);
        if ( prev == m_header_struct.m_leftmost )
            m_header_struct.m_leftmost = to_insert_ptr;
    }
    else
    {
        prev->m_right = std::move (to_insert);
        if ( prev == m_header_struct.m_rightmost )
            m_header_struct.m_rightmost = to_insert_ptr;
    }

    return to_insert_ptr;
}
}   // namespace containers
}   // namespace red