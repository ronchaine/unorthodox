#include "doctest.h"

#include <unorthodox/math/fixed_point.hpp>
#include <unorthodox/math.hpp>

#include <limits>
#include <cmath>
#include <iostream>

TEST_CASE("Minimum and maximum") {
    SUBCASE("Minimum") {
        REQUIRE(unorthodox::minimum(0, 2, 6) == 0);
        REQUIRE(unorthodox::minimum(7.0, -52.1, 60.0) == -52.1);
        REQUIRE(unorthodox::minimum(-1, -532, -2000) == -2000);
        REQUIRE(unorthodox::minimum(-1.0f, -532.0f, -2000.0f) == -2000.0f);
        REQUIRE(unorthodox::minimum(-std::numeric_limits<double>::infinity(), 5.0, 22.2) == -std::numeric_limits<double>::infinity());

    }
    SUBCASE("Maximum") {
        REQUIRE(unorthodox::maximum(0, 2, 6) == 6);
        REQUIRE(unorthodox::maximum(7.0, -52.1, 60.0) == 60.0);
        REQUIRE(unorthodox::maximum(-1, -532, -2000) == -1);
        REQUIRE(unorthodox::maximum(-1.0f, -532.0f, -2000.0f) == -1.0f);
        REQUIRE(unorthodox::maximum(std::numeric_limits<double>::infinity(), 5.0, 22.2) == std::numeric_limits<double>::infinity());
    }
}

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

TEST_CASE("Modulo and remainder") {
    SUBCASE("Modulo") {
        REQUIRE(unorthodox::modulo(1, 1) == 0);
        REQUIRE(unorthodox::modulo(125, 8) == 5);

        REQUIRE(unorthodox::modulo(1.0, 1.0) == 0.0);
        REQUIRE(unorthodox::modulo(125.0, 8.0) == doctest::Approx(std::fmod(125.0, 8.0)));

        // positive should be exact
        REQUIRE(unorthodox::modulo(15.6, 8.52) == 7.08);

        REQUIRE(unorthodox::modulo(42.1, 7.52)   == doctest::Approx(4.5));
        REQUIRE(unorthodox::modulo(-42.1, 7.52)  == doctest::Approx(3.02));
        REQUIRE(unorthodox::modulo(-42.1, -7.52) == doctest::Approx(3.02));
        REQUIRE(unorthodox::modulo(42.1, -7.52)  == doctest::Approx(4.5));
    }
    
    SUBCASE("Remainder") {
        REQUIRE(unorthodox::remainder(1, 1) == 0);
        REQUIRE(unorthodox::remainder(125, 8) == 5);

        REQUIRE(unorthodox::remainder(1.0, 1.0) == 0.0);
        REQUIRE(unorthodox::remainder(125.0, 8.0) == doctest::Approx(std::fmod(125.0, 8.0)));

        // positive should be exact
        REQUIRE(unorthodox::remainder(15.6, 8.52) == 7.08);

        REQUIRE(unorthodox::remainder(42.1, 7.52) == doctest::Approx(std::fmod(42.1, 7.52)));
        REQUIRE(unorthodox::remainder(-42.1, 7.52) == doctest::Approx(std::fmod(-42.1, 7.52)));
        REQUIRE(unorthodox::remainder(-42.1, -7.52) == doctest::Approx(std::fmod(-42.1, -7.52)));
        REQUIRE(unorthodox::remainder(42.1, -7.52) == doctest::Approx(std::fmod(42.1, -7.52)));
    }
}
