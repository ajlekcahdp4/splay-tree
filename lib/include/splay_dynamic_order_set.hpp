/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

// set wrapper just for beauty.

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
    using typename base_do_set::node;
    using typename base_do_set::node_ptr;

  public:
    using typename base_do_set::size_type;
    using typename base_do_set::value_type;

    using typename base_do_set::iterator;
    using typename base_do_set::reverse_iterator;

  private:
    void splay (base_node_ptr node) const
    {
        assert (node);
        while ( node != base_set::root () )
        {
            if ( node->m_parent == base_set::root () )
                node->rotate_to_parent ();
            else
            {
                bool zigzig = node->is_linear ();
                if ( zigzig )
                {
                    node->m_parent->rotate_to_parent ();
                }
                else
                {
                    node->rotate_to_parent ();
                }
                node->rotate_to_parent ();
            }
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
        node::size (left_max) = node::size (left_max->m_left) + node::size (left_max->m_right) + 1;
    }

    void erase (const value_type &key) override {}

  protected:
    void erase_splay (base_node_ptr node)
    {
        assert (node);
        auto to_erase = node;

        if ( base_set::size () > 1 )
        {
            splay (to_erase);
            if ( !to_erase->m_left )
            {
                base_set::leftmost ()       = base_set::root ()->successor ();
                base_set::root ()           = std::move (to_erase->m_right);
                base_set::root ()->m_parent = base_set::root ();
            }
            else if ( !to_erase->m_right )
            {
                base_set::rightmost ()      = base_set::root ()->predecessor ();
                base_set::root ()           = std::move (to_erase->m_left);
                base_set::root ()->m_parent = base_set::root ();
            }

            else
            {
                merge (to_erase->m_left, to_erase->m_right);
            }
        }
        else
        {
            base_set::reset_header_struct ();
        }
        --base_set::size_ref ();
        base_set::erase_node_from_nodes (node);
    }
};

}   // namespace containers
}   // namespace red