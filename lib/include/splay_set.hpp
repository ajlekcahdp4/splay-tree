/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

#pragma once

#include "splay_do_tree.hpp"

namespace red
{
namespace containers
{

template <typename Key_, typename Compare_ = std::less<Key_>>
using splay_set = splay_do_tree<Key_, Compare_>;

}   // namespace containers
}   // namespace red