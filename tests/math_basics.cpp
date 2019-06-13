#include "doctest.h"

#include <unorthodox/math/fixed_point.hpp>
#include <unorthodox/math.hpp>

#include <limits>
#include <cmath>
#include <iostream>

//constexpr static double FLOATING_POINT_TOLERANCE = 2.0e-14;

TEST_CASE("Breaking fractional part") {
    REQUIRE(unorthodox::fract_iec559(0.25) == doctest::Approx(0.25));
    REQUIRE(unorthodox::fract_iec559(0.5) == 0.5);
    REQUIRE(unorthodox::fract_iec559(0.7) == doctest::Approx(0.7));
    REQUIRE(unorthodox::fract_iec559(0.25) == doctest::Approx(0.25));
    REQUIRE(unorthodox::fract_iec559(-0.5) == doctest::Approx(-0.5));
    REQUIRE(unorthodox::fract_iec559(-0.7) == doctest::Approx(-0.7));
    
    REQUIRE(unorthodox::fract_iec559(-64.25) == doctest::Approx(-0.25));
    REQUIRE(unorthodox::fract_iec559(8.3) == doctest::Approx(0.3));
    REQUIRE(unorthodox::fract_iec559(6.7) == doctest::Approx(0.7));
    REQUIRE(unorthodox::fract_iec559(1.25) == doctest::Approx(0.25));
    REQUIRE(unorthodox::fract_iec559(-44.5) == doctest::Approx(-0.5));
    REQUIRE(unorthodox::fract_iec559(-9.7) == doctest::Approx(-0.7));
    
    REQUIRE(unorthodox::fract_iec559(1.0) == doctest::Approx(0.0));
    REQUIRE(unorthodox::fract_iec559(-63.0) == doctest::Approx(0.0));
}
TEST_CASE("Rounding") {
    SUBCASE("positive numbers") {
        REQUIRE(unorthodox::round(0.25) == doctest::Approx(0.0));
        REQUIRE(unorthodox::round(0.5) == doctest::Approx(1.0));
        REQUIRE(unorthodox::round(0.75) == doctest::Approx(1.0));
        
        REQUIRE(unorthodox::round(0.25f) == doctest::Approx(0.0f));
        REQUIRE(unorthodox::round(0.5f) == doctest::Approx(1.0f));
        REQUIRE(unorthodox::round(0.75f) == doctest::Approx(1.0f));
    }
    SUBCASE("negative numbers") {
        REQUIRE(unorthodox::round(-0.25) == doctest::Approx(0));
        REQUIRE(unorthodox::round(-0.5) == doctest::Approx(-1));
        REQUIRE(unorthodox::round(-0.75) == doctest::Approx(-1));
        
        REQUIRE(unorthodox::round(-0.25f) == doctest::Approx(0.0f));
        REQUIRE(unorthodox::round(-0.5f) == doctest::Approx(-1.0f));
        REQUIRE(unorthodox::round(-0.75f) == doctest::Approx(-1.0f));
    }
}

TEST_CASE("Absolute values") {
    REQUIRE(unorthodox::abs(-1) == 1);
    REQUIRE(unorthodox::abs(1) == 1);
    REQUIRE(unorthodox::abs(-1.0) == 1.0);
    REQUIRE(unorthodox::abs(1.0) == 1.0);

    REQUIRE(unorthodox::abs(std::numeric_limits<double>::infinity()) == std::numeric_limits<double>::infinity());
    REQUIRE(unorthodox::abs(-std::numeric_limits<double>::infinity()) == std::numeric_limits<double>::infinity());
    REQUIRE(unorthodox::abs(std::numeric_limits<float>::infinity()) == std::numeric_limits<float>::infinity());
    REQUIRE(unorthodox::abs(-std::numeric_limits<float>::infinity()) == std::numeric_limits<float>::infinity());
}

TEST_CASE("Modulo arithmetics") {
    REQUIRE(unorthodox::modulo(1, 1) == 0);
    REQUIRE(unorthodox::modulo(125, 8) == 5);

    REQUIRE(unorthodox::modulo(1.0, 1.0) == 0.0);
    REQUIRE(unorthodox::modulo(125.0, 8.0) == 5.0);

    // positive should be exact
    REQUIRE(unorthodox::modulo(15.6, 8.52) == doctest::Approx(std::fmod(15.6, 8.52)));

    REQUIRE(std::fabs(unorthodox::modulo(-42.1, 7.52) == doctest::Approx(std::fmod(-42.1, 7.52))));
}
