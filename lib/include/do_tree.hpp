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

    base_node_ptr m_predecessor_for_erase () noexcept;
    base_node_ptr m_successor_for_erase () noexcept;

    base_node_ptr do_tree_increment () noexcept;
    base_node_ptr do_tree_decrement () noexcept;

    base_node_ptr rotate_left ();
    base_node_ptr rotate_right ();

    bool is_left_child () const noexcept
    {
        return (m_parent ? this == m_parent->m_left.get () : false);
    }
};

// Helper type offering value initialization guarantee on the compare functor.
template <class Compare_> struct do_tree_key_compare
{
    Compare_ m_key_compare;

    do_tree_key_compare (const Compare_ &comp) : m_key_compare (comp) {}
    do_tree_key_compare (do_tree_key_compare<Compare_> &&other)
    {
        m_key_compare = std::move (other.m_key_compare);
    }

    template <typename Key> bool operator() (const Key &k1, const Key &k2)
    {
        return m_key_compare (k1, k2);
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

template <typename Val> struct do_tree_node
{
    using size_type = typename do_tree_node_base::size_type;
    using base_node = do_tree_node_base;
    using node_t    = do_tree_node<Val>;
    using node_ptr  = do_tree_node<Val> *;

    do_tree_node (const Val &p_key, size_type p_size = 1) : m_impl {p_size}, m_value {p_key} {}

    base_node m_impl {};
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
    using node_ptr      = typename do_tree_node<Key_t>::node_ptr;
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

        reference operator* () const { return m_node->m_key; }

        pointer get () { return &(m_node->m_key); }

        pointer operator->() { return get (); }

        self &operator++ () noexcept   // pre-increment
        {
            m_node = m_node->do_tree_increment ();
            return *this;
        }

        self operator++ (int) noexcept   // post-increment
        {
            self tmp = *this;
            m_node   = m_node->do_tree_increment ();
            return tmp;
        }

        self &operator-- () noexcept   // pre-decrement
        {
            m_node = (m_node ? m_node->do_tree_decrement () : m_tree->end ());
            return *this;
        }

        self operator-- (int) noexcept   // post-decrement
        {
            self tmp = *this;
            m_node = (m_node ? m_node->do_tree_decrement () : m_tree->m_header_struct.m_rightmost);
            return tmp;
        }

        bool operator== (const self &other) const noexcept { return m_node == other.m_node; }

        bool operator!= (const self &other) const noexcept { return m_node != other.m_node; }

        node_ptr m_node                         = nullptr;
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
    node_ptr root () const noexcept { return m_header_struct.m_header->m_left.get (); }

    iterator m_lower_bound (node_ptr x, node_ptr y, const value_type &k);

    iterator m_upper_bound (node_ptr x, node_ptr y, const value_type &k);

  public:
    do_tree () : m_compare_struct (Compare_t {}), m_header_struct () {}
    do_tree (const Compare_t &comp) : m_compare_struct (comp), m_header_struct () {}

    do_tree (const self &other)         = delete;
    self &operator= (const self &other) = delete;

    do_tree (self &&other) noexcept
        : m_compare_struct (std::move (other.m_compare_struct.m_Keycompare))
    {
        m_header_struct.m_header = std::move (other.m_header_struct.m_header);
        std::swap (m_header_struct.m_leftmost, other.m_header_struct.m_leftmost);
        std::swap (m_header_struct.m_rightmost, other.m_header_struct.m_rightmost);
    }

    // Accessors.

    iterator begin () const noexcept { return iterator (m_header_struct.m_leftmost, this); }

    iterator end () const noexcept { return iterator (nullptr, this); }

    reverse_iterator rbegin () noexcept { return reverse_iterator (end ()); }

    reverse_iterator rend () noexcept { return reverse_iterator (begin ()); }

    size_type size () const noexcept
    {
        auto root = root ();
        return (root ? node::size (root) : 0);
    }

    bool empty () const noexcept { return (size () == 0); }

    template <typename F>
    std::tuple<base_node_ptr, base_node_ptr, bool> m_trav_bin_search (value_type key, F step);

    // Insert/erase.

  private:
    // Insert node in AVL tree without rebalancing.
    base_node_ptr m_insert_node (owning_ptr to_insert);

    // create node, insert and rebalance tree
    iterator m_insert (const value_type &key)
    {
        auto to_insert             = new node (key);
        auto to_insert_base_unique = owning_ptr (static_cast<node_ptr> (to_insert));

        auto res = m_insert_node (std::move (to_insert_base_unique));
        m_rebalance_after_insert (res);

        return iterator (res, this);
    }

    // Rebalance subtree after insert.
    void m_rebalance_after_insert (node_ptr leaf);

    node_ptr m_erase_pos (iterator to_erase_pos) { return m_erase_pos_impl (to_erase_pos); }

    // Rebalance tree for erase.
    void m_rebalance_for_erase (node_ptr node);

    // erase node from the container.
    node_ptr m_erase_pos_impl (iterator pos);

  public:
    iterator find (const value_type &key)
    {
        auto [found, prev, prev_greater] = m_trav_bin_search (key, [] (node_ptr &) {});
        return iterator (found, this);
    }

    iterator m_find_for_erase (const value_type &key)
    {
        auto [found, prev, prev_greater] =
            m_trav_bin_search (key, [] (node_ptr &node) { node->m_size--; });
        auto node = found;

        if ( node )
            return iterator (node, this);

        m_trav_bin_search (key, [] (node_ptr &node) { node->m_size++; });
        throw std::out_of_range ("No element with requested key for erase.");
    }

    iterator insert (const value_type &key) { return m_insert (key); }

    bool erase (const value_type &key)
    {
        auto to_erase_pos = m_find_for_erase (key);
        return m_erase_pos (to_erase_pos);
    }

    void erase (iterator pos)   // ???
    {
        if ( pos != end () )
        {
            m_trav_bin_search (*pos, [] (node_ptr &node) { node->m_size--; });
            m_erase_pos (pos);
        }
    }

    void clear () noexcept { m_header_struct.m_reset (); }

    // Set operations.
    iterator lower_bound (const value_type &k) { return m_lower_bound (root (), nullptr, k); }

    iterator upper_bound (const value_type &k) { return m_upper_bound (root (), nullptr, k); }

    // return key value of ith smallest element in AVL-tree
    value_type m_os_select (size_type i);

    // return the rank of the node with matching Key
    size_type m_get_rank_of (iterator pos);

    // Return number of elements with the key less then the given one.
    size_type m_get_number_less_then (value_type key)
    {
        if ( empty () )
            return 0;

        auto min_key = s_key (begin ());

        if ( this->m_compare_struct.m_Keycompare (key, min_key) || key == min_key )
            return 0;

        /* Previous element exists. */
        auto closest_left = --upper_bound (key);

        auto rank = m_get_rank_of (closest_left);

        return (*closest_left == key ? rank - 1 : rank);
    }

  public:
    bool operator== (const do_tree &other) const
    {
        return size () == other.size () && std::equal (begin (), end (), other.begin ());
    }

    bool operator!= (const do_tree &other) const { return !(*this == other); }

    value_type os_select (size_type i) { return m_os_select (i); }

    size_type get_number_less_then (value_type key) { return m_get_number_less_then (key); }

    void dump (std::string filename) const
    {
        std::ofstream p_stream {filename};
        assert (p_stream);
        p_stream << "digraph {\nrankdir = TB\n";
        for ( auto pos = begin (); pos != end (); pos++ )
        {
            p_stream << "\tnode" << pos.m_node << "[label = \"{" << *pos << " | "
                     << pos.m_node->m_bf << "} | " << pos.m_node->m_size
                     << "\", shape=record, style=filled, fillcolor=palegreen];\n";

            if ( pos.m_node->m_left )
                p_stream << "\tnode" << pos.m_node << " -> node" << pos.m_node->m_left () << ";\n";
            else
            {
                p_stream << "\tnode" << pos.m_node << " -> node0_l_" << *pos << ";\n";
                p_stream << "\tnode0_l_" << *pos
                         << " [label = \"\", shape=triangle, style=filled, fillcolor=black ];\n";
            }

            if ( pos.m_node->m_right )
                p_stream << "\tnode" << pos.m_node << " -> node" << pos.m_node->m_right () << ";\n";
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

typename do_tree_node_base::base_node_ptr do_tree_node_base::m_predecessor_for_erase () noexcept
{
    auto curr = this;

    if ( m_left )
    {
        /*
         * Move down until we find it.
         * Also have to decrease each node size.
         */

        curr = m_left.get ();
        while ( curr->m_right )
        {
            curr->m_size--;
            curr = curr->m_right.get ();
        }
        return curr;
    }

    /* move up until we find it or reach the root. */
    for ( ; curr->m_parent->m_parent && curr->is_left_child (); curr = curr->m_parent )
    {
        ;
    }

    auto parent = curr->m_parent;

    if ( !parent->m_parent ) /* curris a root */
        return nullptr;

    return parent;
}

typename do_tree_node_base::base_node_ptr do_tree_node_base::m_successor_for_erase () noexcept
{
    auto curr = this;

    if ( m_right )
    {
        /*
         * Move down until we find it.
         * Also have to decrease ich node size.
         */

        curr = m_right.get ();
        while ( curr->m_left )
        {
            curr->m_size--;
            curr = curr->m_left.get ();
        }
        return curr;
    }

    /* move up until we find it or reach the root. */
    for ( ; curr->m_parent->m_parent && !curr->is_left_child (); curr = curr->m_parent )
    {
        ;
    }

    auto parent = curr->m_parent;

    if ( !parent->m_parent ) /* curris a root */
        return nullptr;

    return parent;
}

typename do_tree_node_base::base_node_ptr do_tree_node_base::do_tree_increment () noexcept
{
    auto curr = this;
    if ( curr->m_right )

        return curr->m_right->m_minimum ();

    base_node_ptr prev = curr->m_parent;

    /*       while not root      */
    while ( prev->m_parent && !curr->is_left_child () )
    {
        curr = prev;
        prev = prev->m_parent;
    }
    if ( prev->m_parent )
        curr = prev;
    else
        curr = nullptr;
    return curr;
}

typename do_tree_node_base::base_node_ptr do_tree_node_base::do_tree_decrement () noexcept
{
    auto curr = this;

    if ( curr->m_left )
        return curr->m_left->m_maximum ();

    base_node_ptr prev = curr->m_parent;
    /*       while not root      */
    while ( prev->m_parent && curr == prev->m_left.get () )
    {
        curr = prev;
        prev = prev->m_parent;
    }
    if ( prev->m_parent )
        curr = prev;
    else
        curr = nullptr;

    return curr;
}

typename do_tree_node_base::base_node_ptr do_tree_node_base::rotate_left ()
{
    auto node       = this;
    auto parent     = this->m_parent;
    auto rchild_ptr = node->m_right.release ();

    node->m_right = std::move (rchild_ptr->m_left);

    if ( node->m_right )
        node->m_right->m_parent = node;

    rchild_ptr->m_parent = parent;
    if ( node->is_left_child () && parent )
    {
        rchild_ptr->m_left = std::move (parent->m_left);
        parent->m_left     = owning_ptr (rchild_ptr);
    }
    else if ( parent )
    {
        rchild_ptr->m_left = std::move (parent->m_right);
        parent->m_right    = owning_ptr (rchild_ptr);
    }

    node->m_parent = rchild_ptr;

    /* update rchild's and node's sizes (the only sizes changed) */
    rchild_ptr->m_size = node->m_size;
    node->m_size       = self::size (node->m_left.get ()) + self::size (node->m_right.get ()) + 1;

    return rchild_ptr;
}

typename do_tree_node_base::base_node_ptr do_tree_node_base::rotate_right ()
{
    auto node       = this;
    auto parent     = this->m_parent;
    auto lchild_ptr = node->m_left.release ();

    node->m_left = std::move (lchild_ptr->m_right);

    if ( node->m_left )
        node->m_left->m_parent = node;

    lchild_ptr->m_parent = parent;
    if ( node->is_left_child () && parent )
    {
        lchild_ptr->m_right = std::move (parent->m_left);
        parent->m_left      = owning_ptr (lchild_ptr);
    }
    else if ( parent )
    {

        lchild_ptr->m_right = std::move (parent->m_right);
        parent->m_right     = owning_ptr (lchild_ptr);
    }

    node->m_parent = lchild_ptr;

    /* update rchild's and node's sizes (the only sizes changed) */
    lchild_ptr->m_size = node->m_size;
    node->m_size       = self::size (node->m_left.get ()) + self::size (node->m_right.get ()) + 1;

    return lchild_ptr;
}

template <typename Key_t, typename Comp_t>
typename do_tree<Key_t, Comp_t>::value_type do_tree<Key_t, Comp_t>::m_os_select (size_type i)
{
    if ( i > size () || !i )
        throw std::out_of_range ("i is greater then the size of the tree or zero.");

    auto curr = root ();

    /* The rank of node is the size of left subtree plus 1. */
    size_t rank = node::size (curr->m_left ()) + 1;

    while ( rank != i )
    {
        if ( i < rank )
            curr = curr->m_left ();
        else
        {
            curr = curr->m_right ();

            /* Reduce i, cause we've already passed ranksmallest nodes. */
            i -= rank;
        }
        rank = node::size (curr->m_left ()) + 1;
    }

    return s_key (curr);
}
template <typename key, typename Comp_>
typename do_tree<key, Comp_>::size_type do_tree<key, Comp_>::m_get_rank_of (iterator pos)
{
    if ( pos == end () )
        throw std::out_of_range ("Element with the given key is not inserted.");

    auto node = pos.m_node;

    /* The rank of node is the size of left subtree plus 1. */
    size_type rank = node::size (node->m_left ()) + 1;

    while ( node != root () )
    {
        if ( !node->is_left_child () )
            rank += node::size (node->m_parent->m_left ()) + 1;
        node = node->m_parent;
    }

    return rank;
}

template <typename Key_t, typename Comp_t>
template <typename F>
std::tuple<typename do_tree<Key_t, Comp_t>::base_node_ptr,
           typename do_tree<Key_t, Comp_t>::base_node_ptr, bool>
do_tree<Key_t, Comp_t>::m_trav_bin_search (value_type key, F step)
{
    using res = std::tuple<node_ptr, node_ptr, bool>;

    auto prev = root ();
    auto curr = prev;
    if ( !curr )
        return res (nullptr, nullptr, false);

    bool Keyless {};

    while ( curr && s_key (curr) != key )
    {
        Keyless = this->m_compare_struct.m_Keycompare (key, s_key (curr));
        step (curr);
        prev = curr;
        if ( Keyless )
            curr = curr->m_left ();
        else
            curr = curr->m_right ();
    }

    if ( curr == root () )
        step (curr);

    return res (curr, prev, Keyless);
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
    auto [found, prev, prev_greater] = m_trav_bin_search (
        static_cast<node_ptr> (to_insert_ptr)->m_key, [] (node_ptr &node) { node->m_size++; });

    if ( found )
    {
        m_trav_bin_search (static_cast<node_ptr> (to_insert_ptr)->m_key,
                           [] (node_ptr &node) { node->m_size--; });
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

template <typename key, typename Comp_>
typename do_tree<key, Comp_>::node_ptr do_tree<key, Comp_>::m_erase_pos_impl (iterator pos)
{
    auto to_erase   = pos.m_node;
    node_ptr target = nullptr;

    /* choose node's in-order successor if it has two children */
    if ( !to_erase->m_left || !to_erase->m_right )
    {
        target = to_erase;

        /* Change leftmost or rightmost if needed */
        if ( begin () == target )
            begin () = target->m_successor_for_erase ();
        if ( end () == target )
            end () = target->m_predecessor_for_erase ();
    }
    else
    {
        target = to_erase->m_successor_for_erase (); /* to_erase->m_right exist, thus move down */
        std::swap (s_key (target), s_key (to_erase));
    }

    target->m_size--;

    /* rebalancing target subtree */
    m_rebalance_for_erase (target);

    auto child_u_ptr = std::move (target->m_left ? target->m_left : target->m_right);

    if ( child_u_ptr )
        child_u_ptr->m_parent = target->m_parent;

    auto t_parent = target->m_parent;

    if ( target->is_left_child () )
        t_parent->m_left = std::move (child_u_ptr);
    else
        t_parent->m_right = std::move (child_u_ptr);

    return target;
}

template <typename key, typename Comp_>
void do_tree<key, Comp_>::m_rebalance_after_insert (node_ptr node)
{

    /*
     * 1. update the balance factor of parent node;
     * 2. rebalance if the balance factor of parent node temporarily becomes +2 or -2;
     * 3. terminate if the height of that parent subtree remains unchanged.
     */

    auto curr   = node;
    auto parent = curr->m_parent;

    while ( curr != root () )
    {
        int &bf = parent->m_bf;
        if ( curr->is_left_child () ) /* The height of left subtree of parent subtree increases */
        {
            if ( bf == 1 )
            {
                /* The height of parent subtree remains unchanged, thus backtracking terminate. */
                bf = 0;
                break;
            }
            else if ( bf == 0 )
            {
                /*
                 * The height of parent subtree increases by one, thus backtracking continue.
                 * Left-heavy.
                 */
                bf = -1;
            }
            else if ( bf == -1 )
            {
                /* The balance factor becomes -2, thus need to fix imbalance. */
                parent->m_fix_left_imbalance_insert ();
                break;
            }
            else
                throw std::out_of_range ("Unexpected value of bf.");
        }
        else /* The height of right subtree of parent subtree increases */
        {
            if ( bf == -1 )
            {
                /* The height of parent subtree remains unchanged, thus backtracking terminate. */
                bf = 0;
                break;
            }
            else if ( bf == 0 )
            {
                /*
                 * Height of parent subtree increases by one, thus backtracking continue.
                 * Right-heavy.
                 */
                bf = 1;
            }
            else if ( bf == 1 )
            {
                /*
                 * The balance factor becomes 2, thus need to fix imbalance.
                 * After fixing parent tree has the same height, thus backtracking terminate.
                 */
                parent->m_fix_right_imbalance_insert ();
                break;
            }
            else
                throw std::out_of_range ("Unexpected value of bf.");
        }
        curr   = parent;
        parent = curr->m_parent;
    }
}

template <typename key, typename Comp_>
void do_tree<key, Comp_>::m_rebalance_for_erase (node_ptr node)
{

    /*
     * Backtracking algo:
     * 1. update the balance factor of parent node;
     * 2. rebalance if the balance factor of parent node temporarily becomes +2 or -2;
     * 3. terminate if the height of that parent subtree remains unchanged.
     */

    auto curr   = node;
    auto parent = node->m_parent;

    while ( curr != root () )
    {
        auto &parent_bf = parent->m_bf;

        if ( curr->is_left_child () ) /* The height of left subtree of parent subtree decreases. */
        {
            if ( parent_bf == -1 )
                /* The height of parent subtree decreases by one, thus continue backtracking. */
                parent_bf = 0;
            else if ( parent_bf == 0 )
            {
                /* The height of parent subtree remains unchanged, thus backtracking terminate. */
                parent_bf = 1;
                break;
            }
            else if ( parent_bf == 1 )
            {
                /* The balance factor becomes 2, thus need to fix imbalance. */
                parent->m_fix_right_imbalance_erase ();
                break;
            }
            else
                throw std::out_of_range ("Unexpected value of bf.");
        }
        else /* The height of right subtree of parent subtree decreases. */
        {
            if ( parent_bf == 1 )
                /* The height of parent subtree decreases by one, thus continue backtracking. */
                parent_bf = 0;
            else if ( parent_bf == 0 )
            {
                /* The height of parent subtree remains unchanged, thus backtracking terminate. */
                parent_bf = -1;
                break;
            }
            else if ( parent_bf == -1 )
            {
                /* The balance factor becomes -2, thus need to fix imbalance. */
                parent->m_fix_left_imbalance_erase ();
                break;
            }
        }

        curr   = parent;
        parent = curr->m_parent;
    }
}

// Accessors.
template <typename key, typename Comp_>
typename do_tree<key, Comp_>::iterator do_tree<key, Comp_>::m_lower_bound (node_ptr x, node_ptr y,
                                                                           const value_type &k)
{
    while ( x )
    {
        bool Keybigger = this->m_compare_struct.m_Keycompare (s_key (x), k);
        if ( !Keybigger )
        {
            y = x;
            x = x->m_left ();
        }
        else
            x = x->m_right ();
    }
    return iterator (y, this);
}

template <typename key, typename Comp_>
typename do_tree<key, Comp_>::iterator do_tree<key, Comp_>::m_upper_bound (node_ptr x, node_ptr y,
                                                                           const value_type &k)
{
    while ( x )
    {
        bool Keyless = this->m_compare_struct.m_Keycompare (k, s_key (x));
        if ( Keyless )
        {
            y = x;
            x = x->m_left ();
        }
        else
            x = x->m_right ();
    }
    return iterator (y, this);
}
}   // namespace containers

}   // namespace red