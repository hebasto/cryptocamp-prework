#ifndef FAST_EXP_H
#define FAST_EXP_H

#include "calc_squares.h"
#include "mul_mod.h"

#include <cassert>

template <typename T, typename E, T M>
    requires(M >= 1)
T fast_exp(T g, E exp)
{
    assert(exp >= E{0});

    auto squares = calc_squares<T, E, M>(g, exp);
    T result{1};
    for (auto s : squares) {
        result = mul_mod<T, M>(result, s);
    }

    return result;
}

#endif // FAST_EXP_H
