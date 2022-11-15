/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

// splay set with dynamic ordering

#pragma once

#include "dynamic_order_set.hpp"

namespace red
{
namespace containers
{

template <typename T, class Compare_t = std::less<T>>
struct splay_dynamic_order_set : public dynamic_order_set<T, Compare_t>
{
  private:
    using base_set    = base_set<T, Compare_t>;
    using base_do_set = dynamic_order_set<T, Compare_t>;
    using typename base_do_set::base_node;
    using typename base_do_set::base_node_ptr;
    using typename base_do_set::const_node_ptr;
    using typename base_do_set::node;
    using typename base_do_set::node_ptr;

  public:
    using typename base_do_set::size_type;
    using typename base_do_set::value_type;

    using typename base_do_set::const_iterator;
    using typename base_do_set::const_reverse_iterator;
    using typename base_do_set::iterator;
    using typename base_do_set::reverse_iterator;

    using base_set::dump;

  private:
    void splay (base_node_ptr node) const
    {
        assert (node);
        while ( node != base_set::root () )
        {
            if ( node->m_parent != base_set::root () )
            {
                auto to_rotate = node->is_linear () ? node->m_parent : node;
                to_rotate->rotate_to_parent ();
            }
            node->rotate_to_parent ();
        }
    }

    void merge (base_node_ptr left, base_node_ptr right)
    {
        auto left_max = left->maximum ([] (base_node_ptr) {});
        splay (left_max);
        if ( right->is_left_child () )
            left_max->m_right = std::move (right->m_parent->m_left);
        else
            left_max->m_right = std::move (right->m_parent->m_right);
        right->m_parent = left_max;
        if ( left_max->is_left_child () )
            base_set::root () = std::move (left_max->m_parent->m_left);
        else
            base_set::root () = std::move (left_max->m_parent->m_right);
        node::size_ref (left_max) =
            node::size (left_max->m_left) + node::size (left_max->m_right) + 1;
    }

  public:
    void insert (const value_type &key) override
    {
        base_node_ptr to_insert = new node {key};
        base_set::insert_base (
            to_insert, [] (base_node_ptr node) { static_cast<node_ptr> (node)->m_size++; },
            [] (base_node_ptr node) { static_cast<node_ptr> (node)->m_size--; });
        splay (to_insert);
    }

    void erase (const value_type &key) override
    {
        auto [to_erase, prev, prev_greater] =
            base_set::trav_bin_search (key, [] (base_node_ptr) {});
        if ( !to_erase )
            throw std::out_of_range ("Element is not presented");
        erase_splay (to_erase);
    }

    void erase (iterator it) override { erase_splay (it.m_node); }

    iterator find (const value_type key) override
    {
        auto [found, prev, prev_greater] = base_set::trav_bin_search (key, [] (base_node_ptr) {});
        if ( !found )
            return base_set::end ();
        splay (found);
        return iterator {static_cast<node_ptr> (found), this};
    }

    const_iterator find (const value_type key) const override
    {
        auto [found, prev, prev_greater] = base_set::trav_bin_search (key, [] (base_node_ptr) {});
        if ( !found )
            return base_set::end ();
        splay (found);
        return const_iterator {static_cast<node_ptr> (found), this};
    }

    iterator lower_bound (const value_type &key) const override
    {
        auto lb = base_set::lower_bound_base (key);
        if ( lb )
            splay (lb);
        return iterator {static_cast<node_ptr> (lb), this};
    }

    iterator upper_bound (const value_type &key) const override
    {
        auto ub = base_set::upper_bound_base (key);
        if ( ub )
            splay (ub);
        return iterator {static_cast<node_ptr> (ub), this};
    }

  protected:
    void erase_splay (base_node_ptr to_erase)
    {
        assert (to_erase);
        if ( base_set::size () > 1 )
        {
            splay (to_erase);
            if ( !to_erase->m_left )
            {
                base_set::leftmost ()       = base_set::root ()->successor ();
                base_set::root ()           = to_erase->m_right;
                base_set::root ()->m_parent = to_erase->m_parent;
            }
            else if ( !to_erase->m_right )
            {
                base_set::rightmost ()      = base_set::root ()->predecessor ();
                base_set::root ()           = to_erase->m_left;
                base_set::root ()->m_parent = to_erase->m_parent;
            }

            else
            {
                merge (to_erase->m_left, to_erase->m_right);
            }
            base_set::decr_size ();
        }
        else
        {
            base_set::reset_header_struct ();
        }
        base_set::erase_node_from_nodes (to_erase);
    }
};

}   // namespace containers
}   // namespace red