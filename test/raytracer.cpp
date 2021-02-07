#include "catch2/catch.hpp"

#include "math/vec.hpp"

using namespace MjB;

SCENARIO("A tuple with w=1.0 is a point", "[vector][raytracer]") {
    GIVEN("a tuple with a w value of 1.0") {
        vec4f v(4.3, -4.2, 3.1, 1.0);
        THEN("the component values are correct") {
            REQUIRE(is_equal_f(v.x, 4.3));
            REQUIRE(is_equal_f(v.y, -4.2));
            REQUIRE(is_equal_f(v.z, 3.1));
            REQUIRE(is_equal_f(v.w, 1.0));
        }
        AND_THEN("the tuple registers as a point") { REQUIRE(v.is_point()); }
    }
}

SCENARIO("point() produces a point", "[vector][raytracer]") {
    GIVEN("a value produced by point()") {
        auto p = point(4, -4, 3);
        THEN("the value is a point") { REQUIRE(p.is_point()); }
    }
}

SCENARIO("vector() produces a vector", "[vector][raytracer]") {
    GIVEN("a value produced by vector()") {
        auto p = vector(4, -4, 3);
        THEN("the value is a vector") { REQUIRE(p.is_vector()); }
    }
}

SCENARIO("​Adding two tuples", "[vector][raytracer]") {
    GIVEN("a1 <- tuple(3, -2, 5, 1) ​and​ a2 <- tuple(-2, 3, 1, 0)") {
        auto a1 = vec4f(3, -2, 5, 1);
        auto a2 = vec4f(-2, 3, 1, 0);
        THEN("a1 + a2 = tuple(1, 1, 6, 1)") {
            auto result = a1 + a2;
            REQUIRE(result == vec4f(1, 1, 6, 1));
        }
    }
}

SCENARIO("​Subtracting two points", "[vector][raytracer]") {
    GIVEN("p1 <- point(3, 2, 1) ​and​ p2 <_ point(5, 6, 7)") {
        auto p1 = point(3, 2, 1);
        auto p2 = point(5, 6, 7);
        THEN("p1 - p2 = vector(-2, -4, -6)") {
            auto result = p1 - p2;
            REQUIRE(result == vector(-2, -4, -6));
        }
    }
}

SCENARIO("​Negating a tuple", "[vector][raytracer]") {
    GIVEN("​a <- tuple(1, -2, 3, -4)") {
        auto a = vec4f(1, -2, 3, -4);
        THEN("-a = tuple(-1, 2, -3, 4)") { REQUIRE(-a == vec4f(-1, 2, -3, 4)); }
    }
}

SCENARIO("​Multiplying a tuple by a scalar", "[vector][raytracer]") {
    GIVEN("​a <- tuple(1, -2, 3, -4)") {
        auto a = vec4f(1, -2, 3, -4);
        THEN("a * 3.5 = tuple(3.5, -7, 10.5, -14)") {
            auto result = a * 3.5;
            REQUIRE(result == vec4f(3.5, -7, 10.5, -14));
        }
    }
}

SCENARIO("​Multiplying a tuple by a fraction", "[vector][raytracer]") {
    GIVEN("​a <- tuple(1, -2, 3, -4)") {
        auto a = vec4f(1, -2, 3, -4);
        THEN("a * 0.5 = tuple(0.5, -1, 1.5, -2)") {
            auto result = a * 0.5;
            REQUIRE(result == vec4f(0.5, -1, 1.5, -2));
        }
    }
}

SCENARIO("​Dividing a tuple by a scalar", "[vector][raytracer]") {
    GIVEN("​a <- tuple(1, -2, 3, -4)") {
        auto a = vec4f(1, -2, 3, -4);
        THEN("a / 2 = tuple(0.5, -1, 1.5, -2)") {
            auto result = a / 2;
            REQUIRE(result == vec4f(0.5, -1, 1.5, -2));
        }
    }
}

SCENARIO("Computing the magnitude of vector(1, 0, 0)", "[vector][raytracer]") {
    GIVEN("v <- vector(1, 0, 0)") {
        auto v = vector(1, 0, 0);
        THEN("magnitude(v) = 1") { REQUIRE(v.magnitude() == 1); }
    }
}

SCENARIO("Computing the magnitude of vector(0, 1, 0)", "[vector][raytracer]") {
    GIVEN("v <- vector(0, 1, 0)") {
        auto v = vector(0, 1, 0);
        THEN("magnitude(v) = 1") { REQUIRE(v.magnitude() == 1); }
    }
}

SCENARIO("Computing the magnitude of vector(0, 0, 1)", "[vector][raytracer]") {
    GIVEN("​v <- vector(0, 0, 1)") {
        auto v = vector(0, 0, 1);
        THEN("magnitude(v) = 1") { REQUIRE(v.magnitude() == 1); }
    }
}

SCENARIO("Computing the magnitude of vector(1, 2, 3)", "[vector][raytracer]") {
    GIVEN("​v <- vector(1, 2, 3)") {
        auto v = vector(1, 2, 3);
        THEN("magnitude(v) = sqrt(14)") {
            REQUIRE(v.magnitude() == Approx(std::sqrt(14)));
        }
    }
}

SCENARIO("Computing the magnitude of vector(-1, -2, -3)",
         "[vector][raytracer]") {
    GIVEN("​v <- vector(-1, -2, -3)") {
        auto v = vector(-1, -2, -3);
        THEN("magnitude(v) = sqrt(14)") {
            REQUIRE(v.magnitude() == Approx(std::sqrt(14)));
        }
    }
}

SCENARIO("Normalizing vector(4, 0, 0) gives (1, 0, 0)", "[vector][raytracer]") {
    GIVEN("​v <- vector(1, 0, 0)") {
        auto v = vector(4, 0, 0);
        THEN("normalize(v) = vector(1, 0, 0)") {
            REQUIRE(v.normalize() == vector(1, 0, 0));
        }
    }
}

SCENARIO("Normalizing vector(1, 2, 3)", "[vector][raytracer]") {
    GIVEN("​v <- vector(1, 2, 3)") {
        auto v = vector(1, 2, 3);
        THEN(
            "normalize(v) = approximately vector(0.26726, 0.53452, 0.80178) ") {
            REQUIRE(v.normalize() == vector(0.26726, 0.53452, 0.80178));
        }
    }
}

SCENARIO("The magnitude of a normalized vector", "[vector][raytracer]") {
    GIVEN("v <- vector(1, 2, 3)") {
        auto v = vector(1, 2, 3);
        WHEN("norm <- normalize(v)") {
            auto norm = v.normalize();
            THEN("magnitude(norm) == 1") {
                REQUIRE(norm.magnitude() == Approx(1.0f));
            }
        }
    }
}

SCENARIO("The dot product of two tuples", "[vector][raytracer]") {
    GIVEN("​a <- vector(1, 2, 3) and b <- vector(2, 3, 4)") {
        auto a = vector(1, 2, 3);
        auto b = vector(2, 3, 4);

        THEN("dot(a, b) = 20") { REQUIRE(a.dot(b) == Approx(20.0f)); }
    }
}

SCENARIO("The cross product of two vectors", "[vector][raytracer]") {
    GIVEN("​a <- vector(1, 2, 3) and b <- vector(2, 3, 4)") {
        auto a = vector(1, 2, 3);
        auto b = vector(2, 3, 4);

        THEN("cross(a, b) = vector(-1, 2, -1)") {
            REQUIRE(a.cross(b) == vector(-1, 2, -1));
        }
        AND_THEN("cross(b, a) = vector(1, -2, 1)") {
            REQUIRE(b.cross(a) == vector(1, -2, 1));
        }
    }
}