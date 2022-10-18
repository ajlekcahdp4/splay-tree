/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

#pragma once

#include "do_tree.hpp"

#include <cassert>

namespace red
{
namespace containers
{

template <typename Key_t, typename Compare_t = std::less<Key_t>>
struct splay_do_tree : public do_tree<Key_t, Compare_t>
{
  private:
    using base_tree = do_tree<Key_t, Compare_t>;
    using typename base_tree::base_node_ptr;
    using typename base_tree::node;
    using typename base_tree::node_ptr;
    using typename base_tree::owning_ptr;
    using typename base_tree::size_type;
    using typename base_tree::value_type;

    using self = splay_do_tree<Key_t, Compare_t>;

  public:
    using typename base_tree::iterator;

    void splay (base_node_ptr node) const
    {
        assert (node);
        while ( node != base_tree::root () )
        {
            if ( node->m_parent == base_tree::root () )
                node->rotate_to_parent ();
            else
            {
                bool zigzig = node->is_linear ();
                if ( zigzig )
                {
                    node->m_parent->rotate_to_parent ();
                    node->rotate_to_parent ();
                }
                else
                {
                    node->rotate_to_parent ();
                    node->rotate_to_parent ();
                }
            }
        }
    }

    void merge (base_node_ptr left, base_node_ptr right)
    {
        auto left_max = left->m_maximum ();
        this->dump ("merge.dot");
        splay (left_max);
        if ( right->is_left_child () )
            left_max->m_right = std::move (right->m_parent->m_left);
        else
            left_max->m_right = std::move (right->m_parent->m_right);
        right->m_parent                        = left_max;
        if ( left_max->is_left_child () )
            this->m_header_struct.m_header->m_left = std::move (left_max->m_parent->m_left);
        else
            this->m_header_struct.m_header->m_left = std::move (left_max->m_parent->m_right);
        left_max->m_size = left_max->m_left->m_size + left_max->m_right->m_size + 1;
    }

    iterator find (const value_type &value)
    {
        auto [found, prev, prev_greater] =
            base_tree::m_trav_bin_search (value, [] (base_node_ptr) {});
        if ( !found )
            return base_tree::end ();
        splay (found);
        return iterator {found, this};
    }

    base_node_ptr find_node_or_parent (const value_type &value)
    {
        auto [found, prev, prev_greater] =
            base_tree::m_trav_bin_search (value, [] (base_node_ptr) {});
        if ( !found )
        {
            splay (prev);
            return prev;
        }
        splay (found);
        return found;
    }

    void erase (base_node_ptr node)
    {
        assert (node);
        auto to_erase = node;

        if ( base_tree::size () > 1 )
        {
            splay (to_erase);
            if ( !to_erase->m_left )
            {
                base_tree::m_header_struct.m_leftmost = base_tree::root ()->do_tree_increment ();
                base_tree::m_header_struct.m_header->m_left = std::move (to_erase->m_right);
                base_tree::root ()->m_parent = base_tree::m_header_struct.m_header.get ();
            }
            else if ( !to_erase->m_right )
            {
                base_tree::m_header_struct.m_rightmost = base_tree::root ()->do_tree_decrement ();
                base_tree::m_header_struct.m_header->m_left = std::move (to_erase->m_left);
                base_tree::root ()->m_parent = base_tree::m_header_struct.m_header.get ();
            }

            else
            {
                // to_erase->m_right->m_parent = to_erase->m_left->m_parent = nullptr;
                merge (to_erase->m_left.get (), to_erase->m_right.get ());
            }
        }
        else
        {
            base_tree::m_header_struct.m_reset ();
        }
    }

    void erase (const value_type &val)
    {
        auto [to_erase, prev, prev_greater] =
            base_tree::m_trav_bin_search (val, [] (base_node_ptr) {});
        if ( !to_erase )
            throw std::out_of_range ("Element not present");
        erase (to_erase);
    }

    void erase (iterator pos) { erase (*pos); }

    void insert (const value_type &val)
    {
        auto to_insert             = new node {val};
        auto to_insert_base_unique = owning_ptr (static_cast<base_node_ptr> (to_insert));
        auto inserted              = base_tree::m_insert_node (std::move (to_insert_base_unique));
        splay (inserted);
    }

    size_type get_rank_of (base_node_ptr node)
    {
        size_type rank = (node->m_left.get () ? node->m_left->m_size + 1 : 1);
        while ( node != base_tree::root () )
        {
            if ( !node->is_left_child () )
                rank += (node->m_parent->m_left.get () ? node->m_parent->m_left->m_size + 1 : 1);
            node = node->m_parent;
        }
        splay (node);
        return rank;
    }

    size_type get_rank_of (iterator pos) { return get_rank_of (pos.m_node); }

    iterator os_select (size_type p_rank)
    {
        if ( p_rank > base_tree::size () || !p_rank )
            return iterator {nullptr, this};

        auto curr      = base_tree::root ();
        size_type rank = (curr->m_left.get () ? curr->m_left->m_size : 0) + 1;
        while ( rank != p_rank )
        {
            if ( p_rank < rank )
                curr = curr->m_left.get ();
            else
            {
                curr = curr->m_right.get ();
                p_rank -= rank;
            }
            rank = (curr->m_left.get () ? curr->m_left->m_size : 0) + 1;
        }
        return iterator {curr, this};
    }

    size_type get_number_less_then (value_type val)
    {
        if ( base_tree::empty () )
            return 0;
        const auto &min_val =
            static_cast<node_ptr> (base_tree::m_header_struct.m_leftmost)->m_value;
        if ( base_tree::m_compare_struct.m_value_compare (val, min_val) || val == min_val )
            return 0;
        auto closest_left = --upper_bound (val);
        auto rank         = get_rank_of (closest_left);
        return (*closest_left == val ? rank - 1 : rank);
    }

    iterator lower_bound (const value_type &val) const
    {
        base_node_ptr node   = base_tree::root ();
        base_node_ptr parent = nullptr;
        while ( node )
        {
            bool key_bigger = base_tree::m_compare_struct.m_value_compare (
                static_cast<node_ptr> (node)->m_value, val);
            if ( !key_bigger )
            {
                parent = node;
                node   = node->m_left.get ();
            }
            else
                node = node->m_right.get ();
        }
        if ( parent )
            splay (parent);
        return iterator {parent, this};
    }

    iterator upper_bound (const value_type &val) const
    {
        base_node_ptr node   = base_tree::root ();
        base_node_ptr parent = nullptr;
        while ( node )
        {
            bool key_less = base_tree::m_compare_struct.m_value_compare (
                val, static_cast<node_ptr> (node)->m_value);
            if ( key_less )
            {
                parent = node;
                node   = node->m_left.get ();
            }
            else
                node = node->m_right.get ();
        }
        if ( parent )
            splay (parent);
        return iterator {parent, this};
    }
};

}   // namespace containers
}   // namespace red