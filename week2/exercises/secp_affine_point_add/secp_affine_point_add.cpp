#include "secp_coordinate.h"

#include <cassert>
#include <iostream>
#include <string>
#include <string_view>

namespace secp256k1 {

// The secp256k1 curve group element type.
// Represents secp256k1 curve points and the point 0 at infinity.
struct Point {
    // Affine coordinates.
    Coordinate x;
    Coordinate y;

    // The point at infinity.
    static const Point ZERO;

    // The group generator.
    static const Point GENERATOR;

    constexpr Point() : Point(ZERO) {}
    constexpr Point(Coordinate x_, Coordinate y_)
        : x(x_), y(y_)
    {
        assert(y * y == x * x * x + Coordinate{7} || *this == ZERO);
    }
    constexpr Point(std::string_view x_, std::string_view y_)
        : Point(Coordinate{x_}, Coordinate{y_}) {}

    // Implements affine point addition.
    constexpr Point operator+=(const Point& rhs)
    {
        // Case A: `this` is the identity.
        if (*this == ZERO) return *this = rhs;

        // Case B: `rhs` is the identity.
        if (rhs == ZERO) return *this;

        // Case C: Addends are mutual inverses.
        if (x == rhs.x && y != rhs.y) {
            assert(y + rhs.y == Coordinate{0});
            return *this = ZERO;
        }

        Coordinate lambda;
        if (x != rhs.x || y != rhs.y) {
            // Case D: Point addition.
            // https://en.wikipedia.org/wiki/Elliptic_curve_point_multiplication#Point_addition
            lambda = (rhs.y - y) / (rhs.x - x);
        } else {
            // Case E: Point doubling.
            // https://en.wikipedia.org/wiki/Elliptic_curve_point_multiplication#Point_doubling
            lambda = Coordinate{3} * x * x / (Coordinate{2} * y);
        }

        y = lambda * (Coordinate{2} * x + rhs.x - lambda * lambda) - y;
        x = lambda * lambda - x - rhs.x;
        return *this;
    }

    friend Point operator+(Point lhs, const Point& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    // Essentially the same algorithm as used in `Coordinate::pow()`.
    constexpr Point scalar_mul(Scalar s) const
    {
        Point result{ZERO};
        auto doubled = *this;
        while (s) {
            if (s & 1) {
                result += doubled;
            }
            doubled += doubled;
            s >>= 1;
        }
        return result;
    }

    auto operator<=>(const Point&) const = default;
};

inline const Point Point::ZERO{"0", "0"};

inline const Point Point::GENERATOR{
    "0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798",
    "0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"
};

std::ostream& operator<<(std::ostream& os, const Point& p)
{
    return os << "(" << p.x << ", " << p.y << ")";
}

} // namespace secp256k1

int main()
{
    using namespace secp256k1;

    Point p{Coordinate{0} - Coordinate{6}.pow((Coordinate::modulus + 2) / 9), Coordinate{1}};
    std::cout << "p            = " << p << '\n';

    Scalar lambda{"0x5363ad4cc05c30e0a5261c028812645a122e22ea20816678df02967c1b23bd72"};
    std::cout << "lambda       = " << lambda << '\n';
    // std::cout << "lambda ^ 3   = " << lambda.pow(3) << '\n';
    // std::cout << "p            = " << p << '\n';

    // Point rhs = p.scalar_mul(Scalar(5))

    return 0;
}
