#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <vector>

int main(int ac, char *av[])
{
    if (ac != 4) {
        std::cerr << "usage : " << av[0] << " start num_line width\n";
        return 1;
    }

    std::size_t start    = std::strtoul(av[1], nullptr, 10);
    std::size_t num_line = std::strtoul(av[2], nullptr, 10);
    std::size_t width    = std::strtoul(av[3], nullptr, 10);

    std::vector<char> buff;
    buff.reserve(width+1);

    char table[] = { '0','1','2','3','4','5','6','7','8','9',
                     'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r', 's', 't', 'u', 'v', 'w', 'x', 'y','z'
                   };

    for (auto w = 0; w < width; ++w) {
        buff.push_back(table[w % sizeof (table)]);
    }
    buff.push_back('\0');


    for (size_t l = start; l < start+num_line; ++l) {
        std::cout << std::setfill('0') << std::setw(12) << l << " " << &buff[0] << '\n'; // << std::flush;
    }

    return 0;
}
