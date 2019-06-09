#include "doctest.h"

#include <unorthodox/math/fixed_point.hpp>
#include <unorthodox/math.hpp>

#include <limits>
#include <cmath>
#include <iostream>

constexpr static double FLOATING_POINT_TOLERANCE = 2.0e-14;

TEST_CASE("Absolute values") {
    CHECK(unorthodox::abs(-1) == 1);
    CHECK(unorthodox::abs(1) == 1);
    CHECK(unorthodox::abs(-1.0) == 1.0);
    CHECK(unorthodox::abs(1.0) == 1.0);

    CHECK(unorthodox::abs(std::numeric_limits<double>::infinity()) == std::numeric_limits<double>::infinity());
    CHECK(unorthodox::abs(-std::numeric_limits<double>::infinity()) == std::numeric_limits<double>::infinity());
    CHECK(unorthodox::abs(std::numeric_limits<float>::infinity()) == std::numeric_limits<float>::infinity());
    CHECK(unorthodox::abs(-std::numeric_limits<float>::infinity()) == std::numeric_limits<float>::infinity());
}

TEST_CASE("Modulo arithmetics") {
    CHECK(unorthodox::modulo(1, 1) == 0);
    CHECK(unorthodox::modulo(125, 8) == 5);

    CHECK(unorthodox::modulo(1.0, 1.0) == 0.0);
    CHECK(unorthodox::modulo(125.0, 8.0) == 5.0);

    // positive should be exact
    CHECK(unorthodox::modulo(15.6, 8.52) == std::fmod(15.6, 8.52));

    CHECK(std::fabs(unorthodox::modulo(-42.1, 7.52) - std::fmod(-42.1, 7.52)) < FLOATING_POINT_TOLERANCE);
    CHECK(std::fabs(unorthodox::modulo(-42.1, 7.52) - std::fmod(-42.1, 7.52)) < FLOATING_POINT_TOLERANCE);
}
