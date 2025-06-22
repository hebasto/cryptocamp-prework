#ifndef MUL_INV_H
#define MUL_INV_H

#include "../fast_exponentiation/fast_exp.h"
#include "../fast_exponentiation/mul_mod.h"

#include <cassert>

template <typename I, I prime_modulus>
    requires(prime_modulus >= I{2})
I mul_inv(I a)
{
    a = normalize<prime_modulus>(a);
    assert(a != I{0});

    // Using Fermat's little theorem.
    return fast_exp<I, I, prime_modulus>(a, prime_modulus - I{2});
}

#endif // MUL_INV_H
