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

template <typename T, class Compare_t = std::less<T>> struct base_set
{
  private:
    using key_compare_t = key_compare<Compare_t>;
    using self          = base_set<T, Compare_t>;

    using base_node     = dl_binary_tree_node_base;
    using base_node_ptr = base_node *;
    using node          = set_node<T>;
    using node_ptr      = node *;

    using compare = Compare_t;

  public:
    using size_type  = typename std::size_t;
    using value_type = T;

  private:
    key_compare_t m_compare_struct;
    header m_header_struct;

  public:
    base_set () : m_compare_struct {Compare_t {}}, m_header_struct {} {}
    base_set (const Compare_t &comp) : m_compare_struct {comp}, m_header_struct {} {}

    base_set (const self &other)        = delete;
    self &operator= (const self &other) = delete;

    base_set (self &&rhs) noexcept : m_compare_struct {Compare_t {}}
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

    virtual ~base_set () {}

    struct set_iterator
    {
        using iterator_category = typename std::bidirectional_iterator_tag;
        using difference_type   = typename std::ptrdiff_t;
        using value_type        = self::value_type;
        using pointer           = value_type *;
        using reference         = value_type &;

        reference operator* () const { return m_node->m_value; }

        pointer get () { return &m_node->m_value; }

        pointer operator->() { return get (); }

        set_iterator &operator++ ()
        {
            m_node = static_cast<node_ptr> (m_node->successor ());
            return *this;
        }

        set_iterator operator++ (int)
        {
            auto tmp = *this;
            m_node   = static_cast<node_ptr> (m_node->successor ());
            return tmp;
        }

        set_iterator &operator-- ()
        {
            m_node = (m_node ? static_cast<node_ptr> (m_node->predecessor ())
                             : static_cast<node_ptr> (m_tree->m_header_struct.m_rightmost));
            return *this;
        }

        set_iterator operator-- (int)
        {
            auto tmp = *this;
            m_node   = (m_node ? static_cast<node_ptr> (m_node->predecessor ())
                               : static_cast<node_ptr> (m_tree->m_header_struct.m_rightmost));
            return tmp;
        }

        bool operator== (const set_iterator &other) const { return m_node == other.m_node; }

        bool operator!= (const set_iterator &other) const { return !(*this == other); }

        node_ptr m_node    = nullptr;
        const self *m_tree = nullptr;
    };

    using iterator         = set_iterator;
    using reverse_iterator = typename std::reverse_iterator<set_iterator>;

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

    virtual void insert (const value_type &key)
    {
        base_node_ptr to_insert = new node {key};
        auto res                = insert_base (
                           to_insert, [] (base_node_ptr) {}, [] (base_node_ptr) {});
    }

    virtual void erase (const value_type &key)
    {
        auto to_erase = find_for_erase (
            key, [] (base_node_ptr) {}, [] (base_node_ptr) {});
        auto target = update_bounds_for_erase (to_erase, [] (base_node_ptr) {});
        evict_node_for_erase (target);
        m_header_struct.m_nodes.erase (to_erase);
        --m_header_struct.m_size;
    }

    bool equal (const self &other) const
    {
        return size () == other.size () && std::equal (begin (), end (), other.begin ());
    }

  protected:
    base_node_ptr root () const { return m_header_struct.m_header->m_left; }

    template <typename F1, typename F2>
    base_node_ptr insert_base (base_node_ptr to_insert, F1 step, F2 step_if_already)
    {
        insert_node_base (to_insert, step, step_if_already);
        m_header_struct.m_nodes.emplace (to_insert, to_insert);
        ++m_header_struct.m_size;
        return to_insert;
    }

    // insert node in tree without rebalancing.
    // "step" applies to every node on the path to the requested key
    // "step_if_already" applies to all nodes on the path to the requested key if this key has
    // already been inserted
    template <typename F1, typename F2>
    base_node_ptr insert_node_base (base_node_ptr to_insert, F1 step, F2 step_if_already);

    // find node to be erased from the tree
    // "step" applies to every node on the path to the requested key
    // "step_if_no" applies to all nodes on the path to the requested key if there were no
    // element with the requested key
    template <typename F1, typename F2>
    base_node_ptr find_for_erase (const value_type &key, F1 step, F2 step_if_no)
    {
        auto [found, prev, prev_greater] = trav_bin_search (key, step);
        if ( found )
            return found;
        trav_bin_search (key, step_if_no);
        throw std::out_of_range ("No element with requested key to erase");
    }

    // update bounds if we erase bounding value
    // "step" applies to every node on the path to the successor or predecessor.
    template <typename F> base_node_ptr update_bounds_for_erase (base_node_ptr to_erase, F step)
    {
        auto succ            = to_erase->successor_base (step);
        base_node_ptr target = succ;
        if ( m_header_struct.m_leftmost == to_erase )
        {
            m_header_struct.m_leftmost = succ;
            target                     = to_erase;
        }
        if ( m_header_struct.m_rightmost == to_erase )
        {
            m_header_struct.m_rightmost = to_erase->predecessor_base (step);
            target                      = to_erase;
        }
        return target;
    }

    void evict_node_for_erase (base_node_ptr target)
    {
        auto child = target->m_left ? target->m_left : target->m_right;
        if ( child )
            child->m_parent = target->m_parent;
        if ( target->is_left_child () )
            target->m_parent->m_left = child;
        else
            target->m_parent->m_right = child;
    }

    template <typename F>
    std::tuple<base_node_ptr, base_node_ptr, bool> trav_bin_search (value_type key, F step) const;

  public:
    virtual void dump (std::ostream &stream) const
    {
        assert (stream);
        stream << "digraph {\nrankdir = TB\n";
        for ( auto &node : m_header_struct.m_nodes )
        {
            stream << "\tnode" << node.first << "[label = \"" << node.first
                   << "\", shape=record, style=filled, fillcolor=palegreen];\n";

            if ( node.first->m_left )
                stream << "\tnode" << node.first << " -> node" << node.first->m_left
                       << " [color=black, label=\"lchild\"];\n";
            else
            {
                stream << "\tnode" << node.first << " -> node0_l_" << node.first
                       << " [color=black, label=\"lchild\"];\n";
                stream << "\tnode0_l_" << node.first
                       << " [label = \"\", shape=triangle, style=filled, fillcolor=black ];\n";
            }

            if ( node.first->m_right )
                stream << "\tnode" << node.first << " -> node" << node.first->m_right
                       << " [color=black, label=\"rchild\"];\n";
            else
            {
                stream << "\tnode" << node.first << " -> node0_r_" << node.first
                       << " [color=black, label=\"rchild\"];\n";
                stream << "\tnode0_r_" << node.first
                       << " [label = \"\", shape=triangle, style=filled, fillcolor=black];\n";
            }
            stream << "\tnode" << node.first << " -> node" << node.first->m_parent
                   << " [color=red, label=\" parent \"];\n";
        }
        stream << "}\n";
    }
};

template <typename T, typename Compare_t>
bool operator== (const base_set<T, Compare_t> &lhs, const base_set<T, Compare_t> &rhs)
{
    return lhs.equal (rhs);
}

template <typename T, typename Compare_t>
bool operator!= (const base_set<T, Compare_t> &lhs, const base_set<T, Compare_t> &rhs)
{
    return !(lhs == rhs);
}

template <typename Node_t, typename Comp_t>
template <typename F>
std::tuple<typename base_set<Node_t, Comp_t>::base_node_ptr,
           typename base_set<Node_t, Comp_t>::base_node_ptr, bool>
base_set<Node_t, Comp_t>::trav_bin_search (value_type key, F step) const
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
typename base_set<Node_t, Comp_t>::base_node_ptr
base_set<Node_t, Comp_t>::insert_node_base (base_node_ptr to_insert, F1 step, F2 step_if_already)
{
    auto to_insert_ptr = to_insert;
    if ( empty () )
    {
        m_header_struct.m_header->m_left = std::move (to_insert);
        to_insert_ptr->m_parent          = m_header_struct.m_header;
        m_header_struct.m_leftmost       = to_insert_ptr;
        m_header_struct.m_rightmost      = to_insert_ptr;
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