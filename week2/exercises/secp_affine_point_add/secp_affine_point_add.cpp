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

    constexpr Point() : Point(ZERO) {}
    constexpr Point(Coordinate x_, Coordinate y_) : x(x_), y(y_) {}
    constexpr Point(std::string_view x_, std::string_view y_) : x(x_), y(y_) {}

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

inline constexpr Point Point::ZERO{Coordinate{0}, Coordinate{0}};

std::ostream& operator<<(std::ostream& os, const Point& p)
{
    return os << "(" << p.x << ", " << p.y << ")";
}

} // namespace secp256k1

int main()
{
    {
    std::cout << "\nTesting secp256k1 point addition...";

    struct PointAddTestCase {
        std::string a_x, a_y;     // Coordinates of point A.
        std::string b_x, b_y;     // Coordinates of point B.
        std::string sum_x, sum_y; // Coordinates of point (A + B).
    };

    // https://cryptocamp.website/t/secp256k1-part-2-the-secp256k1-group/66#p-143-exercises-11
    const std::vector<PointAddTestCase> point_add_test_vectors{
        {
            "67021774492365321256634043516869791044054964063002935266026048760627130221114", "22817883221438079958217963063610327523693969913024717835557258242342029550595",
            "61124938217888369397608518626468079588341162087856379517664485009963441753645", "5723382937169086635766392599511664586625983027860520036338464885987365575658",
            "78518484088348927894279633941273782106215956054783044881924083038087974375069", "18400956471605157290158330638123206056219981947313880254846397293938760781200"
        },
        {
            "44797955726860071483167773525787460171685721903803276437396496681708013097206", "112878323467240798018200025047246733779416351939079609883282945822975931592141",
            "44797955726860071483167773525787460171685721903803276437396496681708013097206", "2913765770075397405370959961441174073853632726560954156174638184932903079522",
            "0", "0" // Expecting the point at infinity.
        },
        {
            "95200151225387174391707134980196577229773167465894787919263504089948495725202", "94213123740092242124032541289267941722641721980066680728855126898974205181980",
            "95200151225387174391707134980196577229773167465894787919263504089948495725202", "94213123740092242124032541289267941722641721980066680728855126898974205181980",
            "5909177817561749019375996132097716007690336893057112295739767175467136927121", "32162989297956602751967132742255814558956860587998309119003795115938320862381"
        },
        {
            "24050370140998638157368766089090079788245793492514664296883668741529047882113", "14478882322437672032054487172211630444001167135141445302555096737662467817571",
            "15045863282447234231848775263852322721143017336655001075698483887751182719636", "14478882322437672032054487172211630444001167135141445302555096737662467817571",
            "76695855813870323034353443655745505343881173836470898666875431378628604069914", "101313206914878523391516497836476277409268817530499118736902487270246366854092"
        },
        {
            "14256779447437936128616290794341059890063336098474125854681710102809814868320", "90566103014364716248988921534849031279541603477816641946022463390335657035131",
            "2303067510121489830312323422056091166740725427601969990117485452141659178613",  "25225986222951479174582063473838876573728381187823922093435120617573177636532",
            "95430772898311369787541983276504378677140303663720683940530878996024106515165", "48068184564993462938397020947826677061288691733511084479824032705110581338856"
        },
    };

    int i = 1;
    for (const auto& [a_x, a_y, b_x, b_y, sum_x, sum_y] : point_add_test_vectors) {
        std::cout << '\n';

        const secp256k1::Point a{a_x, a_y};
        const secp256k1::Point b{b_x, b_y};
        const auto sum = a + b;
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
    }

    {
    std::cout << "\nTesting secp256k1 scalar multiplication...";

    struct ScalarMulTestCase {
        std::string s;              // Scalar s.
        std::string a_x, a_y;       // Coordinates of point A.
        std::string prod_x, prod_y; // Coordinates of point s·A.
    };

    // https://cryptocamp.website/t/secp256k1-part-2-the-secp256k1-group/66#p-143-exercises-11
    const std::vector<ScalarMulTestCase> scalar_mul_test_vectors{
        {
            "23529072936145521956642440150769408702836782170707519110832596096096916532363",
            "94777218176490725267733209794395406270863807953747235979017564313980479098344", "53121120406880321033414824968851949358991212541220678285657788880408683486672",
            "81492582484984365721511233996054540050314813088236204730182464710703690737195", "84165397430175583340352582740254662715932722835371860159802475562062898918484"
        },
        {
            "77770687059601253501098075906318324640585620643934538062621691587089455400301",
            "5187380010089560191829928600869675928625207216422014112981972591844926771008",  "75026050083095897004323393777174635055491620440662638678606562665317466685019",
            "76999255841974189685876230118581110410155956505185745130247574937430232984638", "87571171775685157828750403037960903210473289232782306139148947195874900187006"
        },
        {
            "3747619523960563074315083315669137577217731866086110333821423552891044218266",
            "66371586610273545144505648512343824229224003523952192165787799288317344396675",  "6489011411151914877089190610663845093649879070897583530615192453262848111419",
            "109441138145498884726545575659592733193661671281368885246963601136369148387669", "83708880322787879701338478937074052809697986569225329829504559758598509123336"
        },
    };

    int i = 1;
    for (const auto& [s, a_x, a_y, prod_x, prod_y] : scalar_mul_test_vectors) {
        std::cout << '\n';

        const secp256k1::Scalar v{s};
        const secp256k1::Point a{a_x, a_y};
        const auto prod = a.scalar_mul(v);
        const secp256k1::Point expected = secp256k1::Point{prod_x, prod_y};
        std::cout << "Test vector " << i++ << ":\n";
        std::cout << "Scalar v           = " << v << '\n';
        std::cout << "Point A            = " << a << '\n';
        std::cout << "v·A (calculated)   = " << prod << '\n';
        std::cout << "v·A (expected)     = " << expected << '\n';

        assert(prod == expected);
    }

    std::cout << '\n';
    }

    return 0;
}
