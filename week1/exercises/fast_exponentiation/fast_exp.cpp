#include "fast_exp.h"

#include <cstdint>
#include <iostream>

int main()
{
    constexpr uint64_t g{3};
    constexpr uint64_t e{218};
    constexpr uint64_t m{1000};
    auto power = fast_exp<std::uint64_t, std::uint64_t, m>(g, e);
    std::cout << "g^e mod m = " << power << '\n';
    return 0;
}
