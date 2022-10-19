#include "splay_set.hpp"

#include <iostream>

int main ()
{
    red::containers::splay_set<int> set {};

    unsigned n_elems {};
    std::cin >> n_elems;
    assert (std::cin.good ());

    for ( ; n_elems > 0; --n_elems )
    {
        int key;
        std::cin >> key;
        assert (std::cin.good ());
        set.insert (key);
    }

    unsigned n_requests {};
    std::cin >> n_requests;
    assert (std::cin.good ());

    for ( ; n_requests > 0; --n_requests )
    {
        int l_bound, r_bound;
        std::cin >> l_bound >> r_bound;
        assert (std::cin.good ());
        if ( r_bound < l_bound )
            throw std::runtime_error ("Invalid request range");

        auto it_l = set.lower_bound (l_bound);
        auto it_r = set.upper_bound (r_bound);

        auto l_rank = (it_l == set.end () ? 0 : set.get_rank_of (it_l));
        auto r_rank = (it_r == set.end () ? 0 : set.get_rank_of (it_r));
        std::cout << r_rank - l_rank << " ";
    }
}
