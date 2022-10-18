#include "splay_set.hpp"

#include <iostream>

int main ()
{
    red::containers::splay_set<int> set {};
    bool not_end = true;

    while ( not_end )
    {
        char query_type {};
        int key;

        if ( !(std::cin >> query_type >> key) )
            break;
        switch ( query_type )
        {
        case 'k':
            set.insert (key);
            break;
        case 'm':
            std::cout << *set.os_select (key) << " ";
            break;
        case 'n':
            std::cout << set.get_number_less_then (key) << " ";
            break;
        default:
            std::cerr << "Invalid query." << std::endl;
            not_end = false;
        }
    }
}
