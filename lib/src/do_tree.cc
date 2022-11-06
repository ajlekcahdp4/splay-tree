/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

// do_tree functions implementation

#include "do_tree.hpp"

namespace red
{
namespace containers
{
typename do_tree_node_base::base_node_ptr do_tree_node_base::do_tree_increment () const
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
        return prev;
    return nullptr;
}

typename do_tree_node_base::base_node_ptr do_tree_node_base::do_tree_decrement () const
{
    auto curr = this;

    if ( curr->m_left )
        return curr->m_left->m_maximum ();

    base_node_ptr prev = curr->m_parent;
    /*       while not root      */
    while ( prev->m_parent && curr == prev->m_left )
    {
        curr = prev;
        prev = prev->m_parent;
    }
    if ( prev->m_parent )
        return prev;
    return nullptr;
}

typename do_tree_node_base::base_node_ptr do_tree_node_base::rotate_left ()
{
    auto node       = this;
    auto parent     = this->m_parent;
    auto rchild_ptr = node;

    node->m_right = std::move (rchild_ptr->m_left);

    if ( node->m_right )
        node->m_right->m_parent = node;

    rchild_ptr->m_parent = parent;
    if ( parent )
    {
        rchild_ptr->m_left = node;
        if ( node->is_left_child () )
            parent->m_left = rchild_ptr;
        else if ( parent )
            parent->m_right = rchild_ptr;
    }
    node->m_parent = rchild_ptr;

    /* update rchild's and node's sizes (the only sizes changed) */
    rchild_ptr->m_size = node->m_size;
    node->m_size       = self::size (node->m_left) + self::size (node->m_right) + 1;

    return rchild_ptr;
}

typename do_tree_node_base::base_node_ptr do_tree_node_base::rotate_right ()
{
    auto node       = this;
    auto parent     = this->m_parent;
    auto lchild_ptr = node;

    node->m_left = std::move (lchild_ptr->m_right);

    if ( node->m_left )
        node->m_left->m_parent = node;

    lchild_ptr->m_parent = parent;
    if ( parent )
    {
        lchild_ptr->m_right = node;
        if ( node->is_left_child () )
            parent->m_left = lchild_ptr;
        else if ( parent )
            parent->m_right = lchild_ptr;
    }

    node->m_parent = lchild_ptr;

    /* update rchild's and node's sizes (the only sizes changed) */
    lchild_ptr->m_size = node->m_size;
    node->m_size       = self::size (node->m_left) + self::size (node->m_right) + 1;

    return lchild_ptr;
}
}   // namespace containers
}   // namespace red