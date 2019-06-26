#ifndef UNORTHODOX_COLOUR_HPP
#define UNORTHODOX_COLOUR_HPP

#include <cstdint>
#include <tuple>
#include <string>

#include "math.hpp"

namespace unorthodox
{
    //! Tag for greyscale colourspace
    struct colourspace_type_greyscale {};
    
    //! Tag for RGB colourspaces (RGBA, BGRA, GRB...)
    /*!
     *  Types with RGB colourspace tag can have a components value of 3 or 4,
     *  depending on whether or not they have an alpha channel
     *
     *  Types with RGB colourspace tag expect to have at least
     *  red_component, green_component and blue_component values,
     *  that designate internal index in their storage
     *
     *  They will also have alpha_component if components == 4
     */
    struct colourspace_type_rgb_tag {};
    
    //! Tag for CIE colourspaces
    struct colourspace_type_cie_tag {};
    
    //! Tag for luma plus chroma colourspaces
    struct colourspace_type_luma_plus_chroma_tag {};
    
    //! Tag for cylinderical colourspaces (HSV, HSL...)
    struct colourspace_type_cylinderical_tag {};
    
    //! Tag for CMYK colourspace
    struct colourspace_type_cmyk_tag {};

    template <typename T> concept ColourFormat = requires(T t)
    {
        // how to interpret the components
        typename T::colourspace_type;

        // needs to define what type each component is stored as
        typename T::component_type;
        typename T::component_index;

        // needs to say how many components
        { T::components };
    };

    struct format_rgba8
    {
        using colourspace_type = colourspace_type_rgb_tag;
        using component_type = uint8_t;
        using component_index = size_t;

        constexpr static int components = 4;

        constexpr static size_t red_component   = 0;
        constexpr static size_t green_component = 1;
        constexpr static size_t blue_component  = 2;
        constexpr static size_t alpha_component = 3;
    };

    struct format_rgb8
    {
        using colourspace_type = colourspace_type_rgb_tag;
        using component_type = uint8_t;
        using component_index = size_t;

        constexpr static int components = 3;

        constexpr static size_t red_component   = 0;
        constexpr static size_t green_component = 1;
        constexpr static size_t blue_component  = 2;
    };

    template <ColourFormat Format = format_rgba8>
    class colour_type
    {
        public:
            constexpr static colour_type rgba(uint32_t);
            constexpr static colour_type rgba(typename Format::component_type,
                                              typename Format::component_type,
                                              typename Format::component_type,
                                              typename Format::component_type);

            constexpr static colour_type rgb(typename Format::component_type,
                                             typename Format::component_type,
                                             typename Format::component_type);

            constexpr static colour_type hsv(double, double, double);

            // Ways to read the value
            constexpr uint32_t      as_rgba_int() const;
            constexpr std::string   as_ansi_code() const;

            constexpr uint8_t red() const;
            constexpr uint8_t green() const;
            constexpr uint8_t blue() const;
            constexpr uint8_t alpha() const;

            constexpr double hue() const;
            constexpr double saturation() const;
            constexpr double value() const;

            constexpr typename Format::component_type component_value(typename Format::component_index index);

        private:
            constexpr colour_type() noexcept = default;

            typename Format::component_type component[Format::components];
    };

    using colour = colour_type<format_rgba8>;

    template <typename T> struct is_colour { constexpr static bool value = false; };
    template <typename T> struct is_colour<colour_type<T>> { constexpr static bool value = true; };

    constexpr inline std::tuple<double, double, double> rgb_to_hsv(double red, double green, double blue);
//    [[ expects: var_in_range(red,         0.0, 1.0)   ]]
//    [[ expects: var_in_range(green,       0.0, 1.0)   ]]
//    [[ expects: var_in_range(blue,        0.0, 1.0)   ]]

    constexpr inline std::tuple<double, double, double> hsv_to_rgb(double hue, double saturation, double value);
//    [[ expects: var_in_range(hue,         0.0, 360.0) ]]
//    [[ expects: var_in_range(saturation,  0.0, 1.0)   ]]
//    [[ expects: var_in_range(value,       0.0, 1.0)   ]]

    // Implementation zone below

    //! Constructor for colour_type, from RGBA unsigned int (e.g. 0xff0000ff)
    /*!
     *  \param in_value     input value as uint32_t
     */
    template <ColourFormat Format>
    constexpr colour_type<Format> colour_type<Format>::rgba(uint32_t in_value)
    {
        if constexpr (std::is_same_v<typename Format::colourspace_type, colourspace_type_rgb_tag>)
        {
            colour_type rval;
            rval.component[Format::red_component]   = (in_value & 0xff000000) >> 24;
            rval.component[Format::green_component] = (in_value & 0x00ff0000) >> 16;
            rval.component[Format::blue_component]  = (in_value & 0x0000ff00) >> 8;
            if constexpr (Format::components == 4)
                rval.component[Format::alpha_component] = (in_value & 0x000000ff);
            return rval;
        }
    }

    template <ColourFormat Format>
    constexpr colour_type<Format> colour_type<Format>::rgba(typename Format::component_type red,
                                                            typename Format::component_type green,
                                                            typename Format::component_type blue,
                                                            typename Format::component_type alpha)
    {
        if constexpr (std::is_same_v<typename Format::colourspace_type, colourspace_type_rgb_tag>)
        {
            colour_type rval;
            rval.component[Format::red_component]   = red;
            rval.component[Format::green_component] = green;
            rval.component[Format::blue_component]  = blue;
            rval.component[Format::alpha_component] = alpha;
            return rval;
        }
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

    // Ways to read the value

    //! Get RGBA integer value of the colour
    template <ColourFormat Format>
    constexpr uint32_t colour_type<Format>::as_rgba_int() const
    {
        return 0x00000000;
    }

    /*
    constexpr std::string   as_ansi_code() const;

    constexpr uint8_t red() const;
    constexpr uint8_t green() const;
    constexpr uint8_t blue() const;
    constexpr uint8_t alpha() const;

    constexpr double hue() const;
    constexpr double saturation() const;
    constexpr double value() const;

    constexpr typename Format::component_type component_value(typename Format::component_index index);
    */

    //! Convert HSV triple to RGB tuple
    /*!
     */
    constexpr inline std::tuple<double, double, double> hsv_to_rgb(double hue, double saturation, double value)
    {
        // TODO: Remove these and uncomment the contracts in the declarations
        assert(var_in_range(hue, 0.0, 360.0));
        assert(var_in_range(saturation, 0.0, 1.0));
        assert(var_in_range(value, 0.0, 1.0));

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

    //! Convert RGB triple to HSV tuple
    /*!
     */
    // TODO: Make the switch from asserts to contracts
    constexpr inline std::tuple<double, double, double> rgb_to_hsv(const double red,
                                                                   const double green,
                                                                   const double blue)
    {
        assert(red >= 0.0 && red <= 1.0);
        assert(green >= 0.0 && green <= 1.0);
        assert(blue >= 0.0 && blue <= 1.0);

        
        double hue = std::numeric_limits<double>::quiet_NaN();
        double saturation = 0.0;

        const double min = minimum(red, green, blue);
        const double max = maximum(red, green, blue);
        const double delta = max - min;

        if ((max == 0.0) || (delta < 0.00001))
            return std::tie(hue, saturation, max);

        saturation = delta / max;

        if (red == max)
            hue = (green - blue) / delta;
        else if (green == max)
            hue = 2.0 + (blue - red) / delta;
        else
            hue = 4.0 + (red - green) / delta;

        hue *= 60.0;

        if (hue < 0.0)
            hue += 360.0;

        return std::tie(hue, saturation, max);
    }

    template <typename T> requires(std::is_integral<T>::value)
    constexpr inline std::tuple<double, double, double> rgb_to_hsv(const T red,
                                                                   const T green,
                                                                   const T blue)
    {
        return rgb_to_hsv(
            (red / static_cast<double>(std::numeric_limits<T>::max))
            (green / static_cast<double>(std::numeric_limits<T>::max))
            (blue / static_cast<double>(std::numeric_limits<T>::max))
        );
    }
}

#endif
