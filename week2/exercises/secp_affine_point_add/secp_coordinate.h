#ifndef SECP_COORDINATE_H
#define SECP_COORDINATE_H

#include <boost/multiprecision/cpp_int.hpp>

#include <cassert>
#include <iostream>
#include <limits>
#include <string_view>
#include <vector>

namespace secp256k1 {

namespace bmp = boost::multiprecision;
using namespace bmp::literals;

class Coordinate {
public:
    using raw_uint = bmp::uint256_t;
    using raw_wide_mul = bmp::uint512_t;
    using raw_wide_add = bmp::number<bmp::cpp_int_backend<257, 257, bmp::unsigned_magnitude, bmp::unchecked, void>>;

    // The coordinate field order p = 2^256 − 2^32 − 977.
    static constexpr raw_uint modulus = 0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f_cppui256;

    Coordinate() = default;
    constexpr explicit Coordinate(const raw_uint& v) : m_value(v) { assert(v < modulus); }
    constexpr explicit Coordinate(raw_uint&& v) : m_value(v) { assert(v < modulus); }
    constexpr explicit Coordinate(uint64_t v) : m_value(v) {}
    constexpr explicit Coordinate(std::string_view value) : m_value(value) {}

    auto operator<=>(const Coordinate&) const = default;

    constexpr Coordinate& operator+=(const Coordinate& rhs)
    {
        m_value = static_cast<raw_uint>((static_cast<raw_wide_add>(m_value) + rhs.m_value) % modulus);
        return *this;
    }

    friend Coordinate operator+(Coordinate lhs, const Coordinate& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    constexpr Coordinate& operator-=(const Coordinate& rhs)
    {
        m_value = static_cast<raw_uint>((static_cast<raw_wide_add>(m_value) + (modulus - rhs.m_value)) % modulus);
        return *this;
    }

    friend Coordinate operator-(Coordinate lhs, const Coordinate& rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    constexpr Coordinate& operator*=(const Coordinate& rhs)
    {
        m_value = static_cast<raw_uint>((static_cast<raw_wide_mul>(m_value) * rhs.m_value) % modulus);
        return *this;
    }

    friend Coordinate operator*(Coordinate lhs, const Coordinate& rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    constexpr Coordinate pow(const Coordinate& exponent) const
    {
        std::vector<Coordinate> squares;
        squares.reserve(std::numeric_limits<raw_uint>::digits);
        auto current_square = *this;
        auto exp = exponent.m_value;
        while (exp) {
            auto next_square = current_square * current_square;
            if (exp & 1) {
                squares.push_back(current_square);
            }
            exp >>= 1;
            current_square = next_square;
        }

        Coordinate result{1};
        for (auto s : squares) {
            result *= s;
        }

        return result;
    }

    constexpr Coordinate& operator/=(const Coordinate& rhs)
    {
        // Using Fermat's little theorem.
        // TODO: Reimplement using extended Euclidean algorithm.
        *this *= rhs.pow(Coordinate{modulus - 2});
        return *this;
    }

    friend Coordinate operator/(Coordinate lhs, const Coordinate& rhs)
    {
        lhs /= rhs;
        return lhs;
    }

    friend std::ostream& operator<<(std::ostream& os, const Coordinate& c) {
        return os << c.m_value;
    }

private:
    raw_uint m_value{0};
};

} // namespace secp256k1

#endif // SECP_COORDINATE_H
