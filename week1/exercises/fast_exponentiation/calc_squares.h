#ifndef CALC_SQUARES_H
#define CALC_SQUARES_H

#include "mul_mod.h"

#include <bit>
#include <cstdint>
#include <type_traits>
#include <vector>

template <std::uint64_t M>
requires (M >= 1)
std::vector<std::uint64_t> calc_squares_impl(std::uint64_t g, std::uint64_t exp) {
    std::vector<std::uint64_t> squares;
    squares.reserve(std::bit_width<std::uint64_t>(exp));
    auto current_square = g % M;
    while (exp) {
        auto next_square = square_mod<std::uint64_t, M>(current_square);
        if (exp & 1) {
            squares.push_back(current_square);
        }
        exp >>= 1;
        current_square = next_square;
    }
    return squares;
}

template <typename T, typename E, T M>
requires (M >= 1)
std::vector<T> calc_squares(T g, E exp) {
    static_assert(std::is_same_v<T, std::uint64_t>, "Types other than uint64_t are not yet supported.");
    return calc_squares_impl<M>(g, exp);
}

#endif // CALC_SQUARES_H
