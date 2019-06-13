#ifndef UNORTHODOX_COLOUR_HPP
#define UNORTHODOX_COLOUR_HPP

#include <cstdint>
#include <tuple>
#include <string>

#include "math.hpp"

namespace unorthodox
{
    struct colourspace_type_grayscale {};
    struct colourspace_type_rgb_tag {};
    struct colourspace_type_cie_tag {};
    struct colourspace_type_luma_plus_chroma_tag {};
    struct colourspace_type_cylinderical_tag {};
    struct colourspace_type_cmyk_tag {};

    template <typename T> concept ColourFormat = requires(T t)
    {
        // how to interpret the components
        typename T::colourspace_type;

        // needs to define what type each component is stored as
        typename T::component_type;

        // needs to say how many components
        { T::components };
    };

    struct format_rgba8
    {
        using colourspace_type = colourspace_type_rgb_tag;
        using component_type = uint8_t;

        constexpr static int components = 4;

        constexpr static size_t red_component   = 0;
        constexpr static size_t green_component = 1;
        constexpr static size_t blue_component  = 2;
        constexpr static size_t alpha_component = 3;
    };

    template <ColourFormat Format>
    class colour_type
    {
        public:
            constexpr static colour_type rgba(uint32_t);
            constexpr static colour_type rgba(typename Format::component_type,
                                              typename Format::component_type,
                                              typename Format::component_type,
                                              typename Format::component_type);

            constexpr static colour_type hsv(double, double, double);

            // Ways to read the value
            constexpr uint32_t      as_rgba_value() const;
            constexpr std::string   as_ansi_code() const;

        private:
            constexpr colour_type() noexcept = default;

            typename Format::component_type component[Format::components];
    };

    using colour = colour_type<format_rgba8>;

    constexpr inline std::tuple<double, double, double> rgb_to_hsv(double red, double green, double blue);
    constexpr inline std::tuple<double, double, double> hsv_to_rgb(double hue, double saturation, double value);

    // Implementation zone below

    template <ColourFormat Format>
    constexpr colour_type<Format> colour_type<Format>::rgba(uint32_t in_value)
    {
        colour_type rval;
        rval.component[Format::red_component]   = (in_value & 0xff000000) >> 24;
        rval.component[Format::green_component] = (in_value & 0x00ff0000) >> 16;
        rval.component[Format::blue_component]  = (in_value & 0x0000ff00) >> 8;
        rval.component[Format::alpha_component] = (in_value & 0x000000ff);
        return rval;
    }
    
    template <ColourFormat Format>
    constexpr colour_type<Format> colour_type<Format>::rgba(typename Format::component_type red,
                                                            typename Format::component_type green,
                                                            typename Format::component_type blue,
                                                            typename Format::component_type alpha)
    {
        colour_type rval;
        rval.component[Format::red_component]   = red;
        rval.component[Format::green_component] = green;
        rval.component[Format::blue_component]  = blue;
        rval.component[Format::alpha_component] = alpha;
        return rval;
    }

    template <ColourFormat Format>
    constexpr colour_type<Format> colour_type<Format>::hsv(double hue, double saturation, double value)
    {
        colour_type rval;
        const auto [red, green, blue] = hsv_to_rgb(hue, saturation, value);

        rval.component[Format::red_component] = 255 * red;
        rval.component[Format::green_component] = 255 * green;
        rval.component[Format::blue_component] = 255 * blue;
        rval.component[Format::alpha_component] = 255;

        return rval;
    }

    constexpr inline std::tuple<double, double, double> hsv_to_rgb(double hue, double saturation, double value)
    {
        const double chroma = value * saturation;
        const double intermediate = chroma * (1.0f - abs(modulo(hue / 60.0, 2.0) -1.0f));
        const double major = value - chroma;

        double red = 0.0;
        double green = 0.0;
        double blue = 0.0;

        if (hue >= 0.0 && hue < 60.0)
        {
            red     = chroma + major;
            green   = intermediate + major;
            blue    = major;
        }
        else if (hue >= 60.0 && hue < 120.0)
        {
            red     = intermediate + major;
            green   = chroma + major;
            blue    = major;
        }
        else if (hue >= 120.0 && hue < 180.0)
        {
            red     = major;
            green   = chroma + major;
            blue    = intermediate + major;
        }
        else if (hue >= 180.0 && hue < 240.0)
        {
            red     = major;
            green   = intermediate + major;
            blue    = chroma + major;
        }
        else if (hue >= 240.0 && hue < 300.0)
        {
            red     = intermediate + major;
            green   = major;
            blue    = chroma + major;
        }
        else if (hue >= 300.0 && hue < 360.0)
        {
            red     = chroma + major;
            green   = major;
            blue    = intermediate + major;
        }

        return std::tie(red, green, blue);
    }

    constexpr inline std::tuple<double, double, double> rgb_to_hsv(double red, double green, double blue)
    {
        double hue = red;
        double saturation = green;
        double value = blue;

        return std::tie(hue, saturation, value);
    }
}

#endif
