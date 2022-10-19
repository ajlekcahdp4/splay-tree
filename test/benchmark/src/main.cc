/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alex.rom23@mail.ru> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Alex Romanov
 * ----------------------------------------------------------------------------
 */

#include "splay_set.hpp"

#include <chrono>
#include <iostream>
#include <set>
#include <utility>
#include <vector>

std::chrono::duration<double, std::milli>
queries_splay (const std::vector<int> &elements, const std::vector<std::pair<int, int>> &bounds)
{
    red::containers::splay_set<int> set {};

    for ( auto elem : elements )
        set.insert (elem);

    auto splay_start = std::chrono::high_resolution_clock::now ();
    for ( auto elem : bounds )
    {
        auto [l_bound, r_bound] = elem;
        auto it_l               = set.lower_bound (l_bound);
        auto it_r               = set.upper_bound (r_bound);

        auto l_rank = (it_l == set.end () ? 0 : set.get_rank_of (it_l));
        auto r_rank = (it_r == set.end () ? 0 : set.get_rank_of (it_r));
        auto res    = r_rank - l_rank;
        asm("" ::"r"(res));
    }
    auto splay_finish = std::chrono::high_resolution_clock::now ();
    return std::chrono::duration<double, std::milli> (splay_finish - splay_start);
}

int get_number_in_range (std::set<int> &set, const int l_bound, const int r_bound)
{
    auto lb = set.lower_bound (l_bound);
    auto ub = set.upper_bound (r_bound);
    return std::distance (lb, ub);
}

std::chrono::duration<double, std::milli>
queries_stl (const std::vector<int> &elements, const std::vector<std::pair<int, int>> &bounds)
{
    std::set<int> set {};

    for ( auto elem : elements )
        set.insert (elem);

    auto stl_start = std::chrono::high_resolution_clock::now ();
    for ( auto elem : bounds )
    {
        auto [l_bound, r_bound] = elem;
        auto res                = get_number_in_range (set, l_bound, r_bound);
        asm("" ::"r"(res));
    }
    auto stl_finish = std::chrono::high_resolution_clock::now ();
    return std::chrono::duration<double, std::milli> (stl_finish - stl_start);
}

std::pair<std::vector<int>, std::vector<std::pair<int, int>>> input ()
{
    unsigned n_elems {};
    std::cin >> n_elems;
    assert (std::cin.good ());

    std::vector<int> elements;
    elements.reserve (n_elems);

    for ( ; n_elems > 0; --n_elems )
    {
        int key;
        std::cin >> key;
        assert (std::cin.good ());
        elements.push_back (key);
    }

    unsigned n_requests {};
    std::cin >> n_requests;
    assert (std::cin.good ());

    std::vector<std::pair<int, int>> bounds;
    bounds.reserve (2 * n_requests);

    for ( ; n_requests > 0; --n_requests )
    {
        int l_bound, r_bound;
        std::cin >> l_bound >> r_bound;
        assert (std::cin.good ());
        assert (l_bound <= r_bound);
        bounds.push_back ({l_bound, r_bound});
    }
    return {elements, bounds};
}

int main ()
{
    auto [elements, bounds] = input ();
    auto splay_duration     = queries_splay (elements, bounds);
    auto stl_duration       = queries_stl (elements, bounds);
    std::cout << "\tred::container::splay_set took " << splay_duration.count () << "ms to run\n";
    std::cout << "\tstd::set took " << stl_duration.count () << "ms to run\n";
    std::cout << std::endl;
}