#include "mul_inv.h"
#include "../fast_exponentiation/mul_mod.h"

#include <iostream>
#include <string>
#include <string_view>

#if !defined(MODULUS)
// Default value.
#define MODULUS 1559
#endif
constexpr IntegerImpl prime_modulus{MODULUS};

IntegerImpl parse_arg(std::string_view arg, std::string_view prefix, IntegerImpl fallback)
{
    if (arg.starts_with(prefix)) {
        return static_cast<IntegerImpl>(std::stoll(std::string(arg.substr(prefix.size()))));
    }
    return fallback;
}

int main(int argc, char* argv[])
{
    // Default value.
    IntegerImpl a{42};

    for (int i = 1; i < argc; ++i) {
        a = parse_arg(argv[i], "a=", a);
    }

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
