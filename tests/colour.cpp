#include "doctest.h"
#include <unorthodox/colour.hpp>

TEST_CASE("HSV to RGB conversions") {
    SUBCASE("Hue 0°-60°") {
        SUBCASE("") {
            auto [red, green, blue] = unorthodox::hsv_to_rgb(0, 0.5, 0.5);
            CHECK(red == 0.5f);
            CHECK(green == 0.25f);
            CHECK(blue == 0.25f);
        }
        SUBCASE("") {
            auto [red, green, blue] = unorthodox::hsv_to_rgb(0, 1.0, 0.5);
            CHECK(red == 0.5f);
            CHECK(green == 0.0f);
            CHECK(blue == 0.0f);
        }
        SUBCASE("") {
            auto [red, green, blue] = unorthodox::hsv_to_rgb(0, 0.0, 0.5);
            CHECK(red == 0.5f);
            CHECK(green == 0.5f);
            CHECK(blue == 0.5f);
        }
        SUBCASE("") {
            auto [red, green, blue] = unorthodox::hsv_to_rgb(15, 0.5, 0.5);
            CHECK(red == 0.5f);
            CHECK(green == 0.3125f);
            CHECK(blue == 0.25f);
        }
        SUBCASE("") {
            auto [red, green, blue] = unorthodox::hsv_to_rgb(30, 1.0, 0.25);
            CHECK(red == 0.25f);
            CHECK(green == 0.125f);
            CHECK(blue == 0.0f);
        }
        SUBCASE("") {
            auto [red, green, blue] = unorthodox::hsv_to_rgb(45, 0.25, 0.5);
            CHECK(red == 0.5f);
            CHECK(green == 0.46875f);
            CHECK(blue == 0.375f);
        }
    }
    SUBCASE("Hue 60°-120°") {
        SUBCASE("") {
            auto [red, green, blue] = unorthodox::hsv_to_rgb(60, 0.5, 0.5);
            CHECK(red == 0.5f);
            CHECK(green == 0.5f);
            CHECK(blue == 0.25f);
        }
        SUBCASE("") {
            auto [red, green, blue] = unorthodox::hsv_to_rgb(75, 1.0, 0.5);
            CHECK(red == 0.375f);
            CHECK(green == 0.5f);
            CHECK(blue == 0.0f);
        }
        SUBCASE("") {
            auto [red, green, blue] = unorthodox::hsv_to_rgb(90, 0.0, 0.5);
            CHECK(red == 0.5f);
            CHECK(green == 0.5f);
            CHECK(blue == 0.5f);
        }
        SUBCASE("") {
            auto [red, green, blue] = unorthodox::hsv_to_rgb(120, 0.5, 0.5);
            CHECK(red == 0.25f);
            CHECK(green == 0.5f);
            CHECK(blue == 0.25f);
        }
    }
    SUBCASE("Hue 120°-360°") {
        SUBCASE("") {
            auto [red, green, blue] = unorthodox::hsv_to_rgb(126, 0.45, 0.65);
            uint8_t ired = red * 255;
            uint8_t igreen = green * 255;
            uint8_t iblue = blue * 255;
            CHECK(ired == 91);
            CHECK(igreen == 165);
            CHECK(iblue == 98);
        }
        SUBCASE("") {
            auto [red, green, blue] = unorthodox::hsv_to_rgb(150, 1.0, 0.25);
            CHECK(red == 0.0f);
            CHECK(green == 0.25f);
            CHECK(blue == 0.125f);
        }
        SUBCASE("") {
            auto [red, green, blue] = unorthodox::hsv_to_rgb(210, 0.5, 0.5);
            CHECK(red == 0.25f);
            CHECK(green == 0.375f);
            CHECK(blue == 0.5f);
        }
        SUBCASE("") {
            auto [red, green, blue] = unorthodox::hsv_to_rgb(280, 0.5, 0.5);
            CHECK(red == doctest::Approx(0.41666666f));
            CHECK(green == 0.25f);
            CHECK(blue == 0.5f);
        }
        SUBCASE("") {
            auto [red, green, blue] = unorthodox::hsv_to_rgb(333, 0.5, 0.5);
            CHECK(red == 0.5f);
            CHECK(green == 0.25f);
            CHECK(blue == doctest::Approx(0.3625));
        }
    }
}

TEST_CASE("RGB to HSV conversions") {
    {
        auto [hue, sat, val] = unorthodox::rgb_to_hsv(0.0, 0.0, 0.0);
        CHECK(isnan(hue));
        CHECK(sat == 0.0);
        CHECK(val == 0.0);
    }
    {
        auto [hue, sat, val] = unorthodox::rgb_to_hsv(1.0, 1.0, 1.0);
        CHECK(isnan(hue));
        CHECK(sat == 0.0);
        CHECK(val == 1.0);
    }
    {
        auto [hue, sat, val] = unorthodox::rgb_to_hsv(1.0, 0.0, 0.0);
        CHECK(hue == 0.0);
        CHECK(sat == 1.0);
        CHECK(val == 1.0);
    }
    {
        auto [hue, sat, val] = unorthodox::rgb_to_hsv(0.0, 1.0, 0.0);
        CHECK(hue == 120.0);
        CHECK(sat == 1.0);
        CHECK(val == 1.0);
    }
    {
        auto [hue, sat, val] = unorthodox::rgb_to_hsv(0.0, 0.0, 1.0);
        CHECK(hue == 240.0);
        CHECK(sat == 1.0);
        CHECK(val == 1.0);
    }
    {
        auto [hue, sat, val] = unorthodox::rgb_to_hsv(0.0, 0.5, 1.0);
        CHECK(hue == 210.0);
        CHECK(sat == 1.0);
        CHECK(val == 1.0);
    }
    {
        auto [hue, sat, val] = unorthodox::rgb_to_hsv(0.1, 0.5, 0.8);
        CHECK(hue == doctest::Approx(205.714));
        CHECK(sat == 0.875);
        CHECK(val == 0.8);
    }
}
