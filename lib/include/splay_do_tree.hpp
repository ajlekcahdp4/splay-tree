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

namespace red
{
namespace containers
{

template <typename Key_t, Compare_t = std::less<Key_t>>
struct splay_do_tree : public do_tree<Key_t, Compare_t>
{
  private:
    using base_tree = do_tree<Key_t, Compare_t>;
    using typename base_tree::base_node_ptr;
    using typename base_tree::node_ptr;
    using self = splay_do_tree<Key_t, Compare_t>;

  public:
    using typename base_tree::iterator;

    void splay (base_node_ptr node) const
    {
        while ( node->m_parent )
        {
            if ( !node->m_parent->m_parent )
                node->rotate_to_parent ();
            else
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

    void merge (base_node_ptr left, base_node_ptr right)
    {
        auto left_max = left->m_maximum ();
        splay (left_max);
        left_max->m_right                      = owning_ptr (right);
        right->m_prent                         = left_max;
        this->m_header_struct.m_header->m_left = owning_ptr (left_max);
        left_max->m_size = left_max->m_left->m_size () + left_max->m_right->m_size () + 1;
    }

    iterator find (const value_type &value)
    {
        auto [found, prev, prev_greater] = this->m_trav_bin_search (value, [] (base_node_ptr) {});
        if ( !found )
            return this->end ();
        splay (found);
        return iterator {found, this};
    }

    void erase (base_node_ptr node)
    {
        auto to_erase = node;

        if ( size () > 1 )
        {
            splay (to_erase);
            if ( !to_erase->m_left )
            {
                this->m_header_struct.m_header->m_left           = std::move (to_erase->m_right);
                this->m_header_struct.m_header->m_left->m_parent = nullptr;
                this->m_header_struct.m_leftmost                 = to_erase->do_tree_increment ();
            }
            else if ( !to_erase->m_right )
            {
                this->m_header_struct.m_header->m_left           = std::move (to_erase->m_left);
                this->m_header_struct.m_header->m_left->m_parent = nullptr;
                this->m_header_struct.m_rightmost                = to_erase->do_tree_decrement ();
            }

            else
            {
                to_erase->m_right->m_parent = to_erase->m_left->m_parent = nullptr;
                merge (to_erase->m_left.release (), to_erase->m_left.release ());
            }
        }
        else
        {
            m_header_struct.m_reset ();
        }
    }

    void erase (const value_type &val)
    {
        auto [to_erase, prev, prev_greater] = m_trav_bin_search (val, [] (base_node_ptr) {});
        if ( !to_erase )
            throw std::out_of_range ("Element not present");
        erase (to_erase);
    }

    void erase (iterator pos) { erase (*pos); }

    void insert (const value_type &val)
    {
        auto to_insert             = new node {val};
        auto to_insert_base_unique = owning_ptr (static_cast<base_node_ptr> (to_insert));
        auto inserted              = m_insert_node (std::move (to_insert_base_unique));
        splay (inserted);
    }

    size_type get_rank_of (base_node_ptr node)
    {
        size_type rank = (node->m_left.get () ? node->m_left->m_size + 1 : 1);
        while ( node != root () )
        {
            if ( !node->is_left_child () )
                rank += (node->m_parent->m_left.get () ? node->m_parent->m_left->m_size + 1 : 1)
                    node = node->m_parent;
        }
        splay (node);
        return rank;
    }

    iterator lower_bound (const value_type &val) const
    {
        base_node_ptr parent = nullptr;
        base_node_ptr node   = root ();

        while ( node )
        {
            bool key_bigger =
                m_compare_struct.m_value_compare (static_cast<node_ptr> (node)->m_value, val);
            if ( !key_bigger )
            {
                parent = node;
                node   = node->m_left.get ();
            }
            else
                node = node->m_right.get ();
        }
        splay (parent);
        return iterator {parent, this};
    }

    iterator upper_bound (const value_type &val) const
    {
        base_node_ptr parent = nullptr;
        base_node_ptr node   = root ();

        while ( node )
        {
            bool key_less =
                m_compare_struct.m_value_compare (val, static_cast<node_ptr> (node)->m_value);
            if ( key_less )
            {
                parent = node;
                node   = node->m_left.get ();
            }
            else
                node = node->m_right.get ();
        }
        splay (parent);
        return iterator {parent, this};
    }
};

}   // namespace containers
}   // namespace red