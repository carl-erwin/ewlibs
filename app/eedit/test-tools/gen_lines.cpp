#include <cstdlib>
#include <iostream>
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
    for (auto w = 0; w < width; ++w) {
            buff.push_back('x');
    }
    buff.push_back('\0');

    for (size_t l = start; l < start+num_line; ++l) {
        std::cout << l << " " << &buff[0] << '\n'; // << std::flush;
    }

    return 0;
}
