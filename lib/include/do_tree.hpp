/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

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

struct do_tree_node_base
{
    using size_type     = std::size_t;
    using base_node_ptr = do_tree_node_base *;
    using self          = do_tree_node_base;
    using owning_ptr    = typename std::unique_ptr<do_tree_node_base>;

    size_type m_size       = 1;
    base_node_ptr m_parent = nullptr;
    owning_ptr m_left      = nullptr;
    owning_ptr m_right     = nullptr;

    virtual ~do_tree_node_base () {}

    static size_type size (base_node_ptr node) { return (node ? node->m_size : 0); }

    base_node_ptr m_minimum () noexcept
    {
        base_node_ptr x = this;
        while ( x->m_left.get () )
            x = x->m_left.get ();
        return x;
    }

    base_node_ptr m_maximum () noexcept
    {
        base_node_ptr x = this;
        while ( x->m_right.get () )
            x = x->m_right.get ();
        return x;
    }

    base_node_ptr do_tree_increment () noexcept;
    base_node_ptr do_tree_decrement () noexcept;

    base_node_ptr rotate_left ();
    base_node_ptr rotate_right ();
    base_node_ptr rotate_to_parent ()
    {
        if ( is_left_child () )
            return m_parent->rotate_right ();
        else
            return m_parent->rotate_left ();
    }

    bool is_left_child () const noexcept
    {
        return (m_parent ? this == m_parent->m_left.get () : false);
    }

    bool is_linear () const noexcept
    {
        return ((is_left_child () && m_parent->is_left_child ()) ||
                (!is_left_child () && !m_parent->is_left_child ()));
    }
};
// Helper type offering value initialization guarantee on the compare functor.
template <class Compare_> struct do_tree_key_compare
{
    Compare_ m_value_compare;

    do_tree_key_compare (const Compare_ &comp) : m_value_compare (comp) {}

    template <typename Key> bool operator() (const Key &k1, const Key &k2)
    {
        return m_value_compare (k1, k2);
    }
};

// Helper type to manage deafault initialization of node count and header.
struct do_tree_header
{
    using base_node_ptr = do_tree_node_base *;
    using owning_ptr    = typename do_tree_node_base::owning_ptr;
    using base_node     = do_tree_node_base;

    owning_ptr m_header       = nullptr;
    base_node_ptr m_leftmost  = nullptr;
    base_node_ptr m_rightmost = nullptr;

    do_tree_header ()
    {
        m_header = std::make_unique<base_node> ();
        m_reset ();
    }

    void m_reset () noexcept
    {
        m_header->m_parent = nullptr;
        m_leftmost         = nullptr;
        m_rightmost        = nullptr;
        /* delete tree of unique pointers if any*/
        m_header->m_left.release ();
    }
};

template <typename Val> struct do_tree_node : public do_tree_node_base
{
    using size_type = typename do_tree_node_base::size_type;
    using base_node = do_tree_node_base;
    using node_t    = do_tree_node<Val>;
    using node_ptr  = do_tree_node<Val> *;

    do_tree_node (const Val &p_key) : m_value {p_key} {}

    Val m_value {};
};

//=================================do_tree_=======================================
template <typename Key_t, class Compare_t = std::less<Key_t>> struct do_tree
{
    using key_compare = do_tree_key_compare<Compare_t>;
    using header      = do_tree_header;
    using self        = do_tree<Key_t, Compare_t>;

    using base_node     = do_tree_node_base;
    using base_node_ptr = do_tree_node_base *;
    using node_ptr      = do_tree_node<Key_t> *;
    using owning_ptr    = typename do_tree_node_base::owning_ptr;

    using node = do_tree_node<Key_t>;

    key_compare m_compare_struct;
    header m_header_struct;

    struct do_tree_iterator
    {
        using value_type = Key_t;
        using reference  = Key_t &;
        using pointer    = Key_t *;

        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = std::ptrdiff_t;

        using self = do_tree_iterator;

        reference operator* () const { return (static_cast<node_ptr> (m_node))->m_value; }

        pointer get () { return &(static_cast<node_ptr> (m_node)->m_value); }

        pointer operator->() { return get (); }

        self &operator++ () noexcept
        {
            m_node = m_node->do_tree_increment ();
            return *this;
        }

        self operator++ (int) noexcept
        {
            self tmp = *this;
            m_node   = m_node->do_tree_increment ();
            return tmp;
        }

        self &operator-- () noexcept
        {
            m_node = (m_node ? m_node->do_tree_decrement () : m_tree->m_header_struct.m_rightmost);
            return *this;
        }

        self operator-- (int) noexcept
        {
            self tmp = *this;
            m_node = (m_node ? m_node->do_tree_decrement () : m_tree->m_header_struct.m_rightmost);
            return tmp;
        }

        bool operator== (const self &other) const noexcept { return m_node == other.m_node; }

        bool operator!= (const self &other) const noexcept { return m_node != other.m_node; }

        base_node_ptr m_node                    = nullptr;
        const do_tree<Key_t, Compare_t> *m_tree = nullptr;
    };

  public:
    using value_type = Key_t;
    using pointer    = value_type *;
    using reference  = value_type &;

    using iterator         = do_tree_iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;

    using size_type = typename node::size_type;

  private:
    base_node_ptr root () const noexcept { return m_header_struct.m_header->m_left.get (); }

  public:
    do_tree () : m_compare_struct (Compare_t {}), m_header_struct () {}
    do_tree (const Compare_t &comp) : m_compare_struct (comp), m_header_struct () {}

    do_tree (const self &other)         = delete;
    self &operator= (const self &other) = delete;

    do_tree (self &&rhs) noexcept : m_compare_struct {Compare_t {}}
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

    virtual ~do_tree () {}

    // Accessors.

    iterator begin () const noexcept { return iterator {m_header_struct.m_leftmost, this}; }

    iterator end () const noexcept { return iterator {nullptr, this}; }

    reverse_iterator rbegin () noexcept { return reverse_iterator {end ()}; }

    reverse_iterator rend () noexcept { return reverse_iterator {begin ()}; }

    size_type size () const noexcept
    {
        auto root = this->root ();
        return (root ? node::size (root) : 0);
    }

    bool empty () const noexcept { return (size () == 0); }

    template <typename F>
    std::tuple<base_node_ptr, base_node_ptr, bool> m_trav_bin_search (value_type key, F step);

  private:
    // actually insert node in AVL
    base_node_ptr m_insert_node (owning_ptr to_insert);

  public:
    iterator insert (const value_type &key)
    {
        auto to_insert             = new node (key);
        auto to_insert_base_unique = owning_ptr (static_cast<node_ptr> (to_insert));

        auto res = m_insert_node (std::move (to_insert_base_unique));

        return iterator {res, this};
    }

    void clear () noexcept { m_header_struct.m_reset (); }

  public:
    bool operator== (const do_tree &other) const
    {
        return size () == other.size () && std::equal (begin (), end (), other.begin ());
    }

    bool operator!= (const do_tree &other) const { return !(*this == other); }

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
                p_stream << "\tnode" << pos.m_node << " -> node" << pos.m_node->m_left.get ()
                         << ";\n";
            else
            {
                p_stream << "\tnode" << pos.m_node << " -> node0_l_" << *pos << ";\n";
                p_stream << "\tnode0_l_" << *pos
                         << " [label = \"\", shape=triangle, style=filled, fillcolor=black ];\n";
            }

            if ( pos.m_node->m_right )
                p_stream << "\tnode" << pos.m_node << " -> node" << pos.m_node->m_right.get ()
                         << ";\n";
            else
            {
                p_stream << "\tnode" << pos.m_node << " -> node0_r_" << *pos << ";\n";
                p_stream << "\tnode0_r_" << *pos
                         << " [label = \"\", shape=triangle, style=filled, fillcolor=black];\n";
            }
        }
        p_stream << "}\n";
    }
};

template <typename Key_t, typename Comp_t>
template <typename F>
std::tuple<typename do_tree<Key_t, Comp_t>::base_node_ptr,
           typename do_tree<Key_t, Comp_t>::base_node_ptr, bool>
do_tree<Key_t, Comp_t>::m_trav_bin_search (value_type key, F step)
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
            this->m_compare_struct.m_value_compare (key, static_cast<node_ptr> (curr)->m_value);
        step (curr);
        prev = curr;
        if ( key_less )
            curr = curr->m_left.get ();
        else
            curr = curr->m_right.get ();
    }

    if ( curr == root () )
        step (curr);

    return res {curr, prev, key_less};
}

template <typename Key_t, typename Comp_t>
typename do_tree<Key_t, Comp_t>::base_node_ptr
do_tree<Key_t, Comp_t>::m_insert_node (owning_ptr to_insert)
{
    auto to_insert_ptr = to_insert.get ();
    if ( empty () )
    {
        m_header_struct.m_header->m_left = std::move (to_insert);
        to_insert_ptr->m_parent          = m_header_struct.m_header.get ();

        m_header_struct.m_leftmost  = to_insert_ptr;
        m_header_struct.m_rightmost = to_insert_ptr;

        return to_insert_ptr;
    }

    /* Find right position in the tree */
    auto [found, prev, prev_greater] =
        m_trav_bin_search (static_cast<node_ptr> (to_insert_ptr)->m_value,
                           [] (base_node_ptr node) { node->m_size++; });

    if ( found )
    {
        m_trav_bin_search (static_cast<node_ptr> (to_insert_ptr)->m_value,
                           [] (base_node_ptr node) { node->m_size--; });
        throw std::out_of_range ("Element already inserted");
    }
    to_insert->m_parent = prev;

    if ( prev == m_header_struct.m_header.get () || prev_greater )
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