#ifndef MUL_MOD_H
#define MUL_MOD_H

#include <cassert>
#include <cstdint>
#include <limits>
#include <type_traits>

// More effective platform-specific implementations
// are possible (__uint128_t, _umul128).
using IntegerImpl = std::int64_t;
static constexpr std::size_t INTEGER_IMPL_BITS{63};

template <IntegerImpl modulus>
IntegerImpl normalize(IntegerImpl a)
{
    a %= modulus;
    return a < 0 ? a + modulus : a;
}

template <IntegerImpl modulus>
    // Limit modulus value to prevent addition overflow.
    requires(1 <= modulus && modulus <= std::numeric_limits<IntegerImpl>::max() / 2)
IntegerImpl mul_mod_impl(IntegerImpl a, IntegerImpl b)
{
    IntegerImpl result{0};
    a = normalize<modulus>(a);
    assert(a >= 0);
    b = normalize<modulus>(b);
    assert(b >= 0);

    while (b) {
        if (b & 1) {
            result = (result + a) % modulus;
        }
        a = (a + a) % modulus;
        b >>= 1;
    }

    assert(result >= 0);
    return result;
}

template <typename I, I modulus>
    requires(modulus >= 1)
I mul_mod(I a, I b)
{
    static_assert(std::is_same_v<I, IntegerImpl>);
    return mul_mod_impl<modulus>(a, b);
}

template <typename I, I modulus>
I square_mod(I a)
{
    return mul_mod<I, modulus>(a, a);
}

#endif // MUL_MOD_H
