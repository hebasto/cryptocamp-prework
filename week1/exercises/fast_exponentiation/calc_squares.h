#ifndef CALC_SQUARES_H
#define CALC_SQUARES_H

#include "mul_mod.h"

#include <vector>

template <IntegerImpl M>
std::vector<IntegerImpl> calc_squares_impl(IntegerImpl g, IntegerImpl exp)
{
    std::vector<IntegerImpl> squares;
    squares.reserve(INTEGER_IMPL_BITS);
    auto current_square = g % M;
    while (exp) {
        auto next_square = square_mod<IntegerImpl, M>(current_square);
        if (exp & 1) {
            squares.push_back(current_square);
        }
        exp >>= 1;
        current_square = next_square;
    }
    return squares;
}

template <typename T, typename E, T M>
std::vector<T> calc_squares(T g, E exp)
{
    return calc_squares_impl<M>(g, exp);
}

#endif // CALC_SQUARES_H
