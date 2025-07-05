#include "secp_coordinate.h"

#include <cassert>
#include <iostream>
#include <string_view>

namespace secp256k1 {

// The secp256k1 curve group element type.
// Represents secp256k1 curve points and the point 0 at infinity.
struct Point {
    // Affine coordinates.
    Coordinate x;
    Coordinate y;

    // Jacobian coordinates.
    Coordinate X;
    Coordinate Y;
    Coordinate Z;

    // The point at infinity.
    static const Point ZERO;

    // The group generator.
    static const Point GENERATOR;

    constexpr Point() : Point(ZERO) {}

    // Affine coordinate constructors.
    constexpr Point(Coordinate ax, Coordinate ay)
        : x(ax), y(ay), X(ax), Y(ay), Z(1) {}
    constexpr Point(std::string_view ax, std::string_view ay)
        : Point(Coordinate{ax}, Coordinate{ay}) {}

    // Jacobian coordinate constructors.
    constexpr Point(Coordinate jx, Coordinate jy, Coordinate jz)
        : X(jx), Y(jy), Z(jz) {}
    constexpr Point(std::string_view jx, std::string_view jy, std::string_view jz)
        : Point(Coordinate{jx}, Coordinate{jy}, Coordinate{jz})
    {
        TransformJacobianToAffine();
    }

    constexpr void TransformJacobianToAffine()
    {
        auto zz = Z * Z;
        x = X / zz;
        y = Y / (zz * Z);
    }

    // Implements Jacobian point addition.
    constexpr Point operator+=(const Point& rhs)
    {
        // Case A: `this` is the identity.
        if (*this == ZERO) return *this = rhs;

        // Case B: `rhs` is the identity.
        if (rhs == ZERO) return *this;

        // Case C: Point addition.
        // https://en.wikibooks.org/wiki/Cryptography/Prime_Curve/Jacobian_Coordinates#Point_Addition_(12M_+_4S)
        auto zz1 = Z * Z;
        auto zz2 = rhs.Z * rhs.Z;
        auto U = X * zz2;
        auto H = rhs.X * zz1 - U;
        auto S = Y * zz2 * rhs.Z;
        auto R = rhs.Y * zz1 * Z - S;
        if (H != Coordinate(0)) {
            auto hh = H * H;
            auto hhh = hh * H;
            X = R * R - hhh - Coordinate(2) * U * hh;
            Y = R * (U * hh - X) - S * hhh;
            Z *= H * rhs.Z;
            return *this;
        }

        // Case D: Addends are mutual inverses.
        if (R != Coordinate(0)) return *this = ZERO;

        // Case E: Point doubling.
        // https://en.wikibooks.org/wiki/Cryptography/Prime_Curve/Jacobian_Coordinates#Point_Doubling_(4M_+_6S_or_4M_+_4S)
        auto yy = Y * Y;
        auto T = Coordinate(4) * X * yy;
        auto M = Coordinate(3) * X * X;
        Z = Coordinate(2) * Y * Z;
        X = M * M - Coordinate(2) * T;
        Y = M * (T - X) - Coordinate(8) * yy * yy;
        return *this;
    }

    friend constexpr Point operator+(Point lhs, const Point& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    constexpr bool operator==(const Point& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }
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
    struct TestCase {
        std::string_view a_x, a_y, a_z; // Jacobian coordinates of point A.
        std::string_view b_x, b_y, b_z; // Jacobian coordinates of point B.
        std::string_view sum_x, sum_y;  // Affine coordinates of point (A + B).
    };

    const std::vector<TestCase> test_vectors{
        {
            "61168739479711927142764658335960185139044138470269152817362835609619277248733", "21365265259791813296359020025112135293342760115353080382870338561918313862807", "37064183328797598544560694959943799168750358913858865780091974718018553562419",
            "75776791705958340557958402430698975706422201066979121642449913138944604425660", "66383280047496136929271400526347103822935621943780462161181840552194350141564", "75975606300704613123930174557625573844043347281105167940536468038500802717509",
            "72863032945283280953636129059545959529634042357753453132026174732744194676931", "111529132148508388427246132585785101600429639308058372390604751264868469767543"
        },
        {
            "89959325059742944430358451400705002920926825355225869621717936807494095714290", "96093053924735119484524007701924861311484651710593769022900107977673928960245",  "66142611799578950251083409575885695298839488135797694779041885661190727675299",
            "61152793683249667605361745755257610395039301799655537107480658643593848781730", "108824838086741573141078213715633247883899533027170274847878148878014138167046", "20026567909062914103680712539641599080083135680565932483453732406779235372092",
            "0", "0" // Expecting the point at infinity.
        },
        {
            "1547568827951595983041825486208171785819741431893371520256763714464258127790",   "87153109579099129796596751254693228766379983077346253255841414029284516911078", "105104885998309941273615701006706417602105584887217436384728254947105995740715",
            "102754269592907928248165438489539780821724369832426272173645274109108284691770", "38298190034438650883752719589335983487411860447931052099125319988280170002045", "56745928453254477537417735654158445415425453625586007664329168279192608303666",
            "21324256287414615615026299379536579336529998865990184416926039607504524853626",  "96719670966356830360698314514227297774284915420887284954650836535688914930874"
        },
    };

    int i = 1;
    for (const auto& [a_x, a_y, a_z, b_x, b_y, b_z, sum_x, sum_y] : test_vectors) {
        std::cout << '\n';

        const secp256k1::Point a{a_x, a_y, a_z};
        const secp256k1::Point b{b_x, b_y, b_z};
        auto sum = a + b;
        sum.TransformJacobianToAffine();
        const secp256k1::Point expected = (sum_x == "0" && sum_y == "0") ? secp256k1::Point::ZERO
                                                                         : secp256k1::Point{sum_x, sum_y};
        std::cout << "Test vector " << i++ << ":\n";
        std::cout << "Point A            = " << a << '\n';
        std::cout << "Point B            = " << b << '\n';
        std::cout << "A + B (calculated) = " << sum << '\n';
        std::cout << "A + B (expected)   = " << expected << '\n';

        assert(sum == expected);
    }

    std::cout << '\n';

    return 0;
}
