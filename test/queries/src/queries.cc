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
        std::cout << set.get_number_less_then (r_bound) - set.get_number_less_then (l_bound) << " ";
    }
}
