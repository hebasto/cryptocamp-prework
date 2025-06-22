#ifndef MUL_MOD_H
#define MUL_MOD_H

#include <cstdint>
#include <limits>
#include <type_traits>

template <std::uint64_t M>
// Limit maximum M to prevent addition overflow.
requires (1 <= M && M <= std::numeric_limits<std::uint64_t>::max() / 2)
std::uint64_t mul_mod_impl(std::uint64_t a, std::uint64_t b) {
    // More effective platform-specific implementations
    // are possible (__uint128_t, _umul128).
    std::uint64_t result{0};
    a %= M;
    while (b) {
        if (b & 1) {
            result = (result + a) % M;
        }
        a = (a + a) % M;
        b >>= 1;
    }
    return result;
}

template <typename T, T M>
requires (M >= 1)
T mul_mod(T a, T b) {
    static_assert(std::is_same_v<T, std::uint64_t>, "Types other than uint64_t are not yet supported.");
    return mul_mod_impl<M>(a, b);
}

template <typename T, T M>
requires (M >= 1)
T square_mod(T a) {
    return mul_mod<T, M>(a, a);
}

#endif // MUL_MOD_H
