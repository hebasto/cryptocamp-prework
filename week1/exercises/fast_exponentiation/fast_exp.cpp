#include "fast_exp.h"

#include "../helper/args_parser.h"

#include <iostream>

#define DEFAULT_MODULUS 1559
#if !defined(MODULUS)
#define MODULUS DEFAULT_MODULUS
#endif
constexpr IntegerImpl prime_modulus{MODULUS};

int main(int argc, char* argv[])
{
    const auto a = parse_int_args<IntegerImpl>(argc, argv, "a=", /* default base */ 42);
    const auto exp = parse_int_args<IntegerImpl>(argc, argv, "e=", /* default exponent */ 1024);
    const auto power = fast_exp<IntegerImpl, IntegerImpl, prime_modulus>(a, exp);

    std::cout << "Base a       = " << a << '\n';
    std::cout << "Exponent e   = " << exp << '\n';
    std::cout << "Modulus m    = " << prime_modulus << "\n\n";
    std::cout << "a ^ e        ≡ " << power << " (mod m)\n\n";

    return 0;
}
