#include "mul_inv.h"

#include "../fast_exponentiation/mul_mod.h"
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

    std::cout << "Modulus p    = " << prime_modulus << " (assumed prime; not validated)\n";

    std::cout << "\nBasic scenario:\n";
    std::cout << "a            = " << a << '\n';
    const auto inv = mul_inv<IntegerImpl, prime_modulus>(a);
    std::cout << "inv(a)       = " << inv << '\n';
    // Verify: a * inv(a) ≡ 1 (mod p)
    std::cout << "a * inv(a)   ≡ " << mul_mod<IntegerImpl, prime_modulus>(a, inv) << " (mod p)\n";
    // Verify: inv(a) * a ≡ 1 (mod p)
    std::cout << "inv(a) * a   ≡ " << mul_mod<IntegerImpl, prime_modulus>(inv, a) << " (mod p)\n";

    std::cout << "\nAdditional scenario, negated a:\n";
    std::cout << "-a           = " << -a << '\n';
    const auto inv_negated = mul_inv<IntegerImpl, prime_modulus>(-a);
    std::cout << "inv(-a)      = " << inv_negated << '\n';
    // Verify: -a * inv(-a) ≡ 1 (mod p)
    std::cout << "-a * inv(-a) ≡ " << mul_mod<IntegerImpl, prime_modulus>(-a, inv_negated) << " (mod p)\n";
    // Verify: inv(-a) * -a ≡ 1 (mod p)
    std::cout << "inv(-a) * -a ≡ " << mul_mod<IntegerImpl, prime_modulus>(inv_negated, -a) << " (mod p)\n";

    return 0;
}
