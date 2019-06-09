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
        SUBCASE("") {
            auto [red, green, blue] = unorthodox::hsv_to_rgb(150, 1.0, 0.25);
            CHECK(red == 0.0f);
            CHECK(green == 0.25f);
            CHECK(blue == 0.125f);
        }
    }
    SUBCASE("Hue 120°-360°") {
        SUBCASE("") {
            auto [red, green, blue] = unorthodox::hsv_to_rgb(126, 0.45, 0.65);
            uint8_t ired = red * 255;
            uint8_t igreen = green * 255;
            uint8_t iblue = blue * 255;
            CHECK(ired == 91);
            CHECK(igreen == 166);
            CHECK(iblue == 99);
        }
        SUBCASE("") {
            auto [red, green, blue] = unorthodox::hsv_to_rgb(60, 0.5, 0.5);
            CHECK(red == 0.5f);
            CHECK(green == 0.5f);
            CHECK(blue == 0.25f);
        }
    }
}
