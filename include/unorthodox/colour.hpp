#ifndef UNORTHODOX_COLOUR_HPP
#define UNORTHODOX_COLOUR_HPP

// -std=c++2a -Xclang -fconcepts-ts

namespace unorthodox
{
    template <typename T> concept ColourFormat = requires(T t)
    {
        // needs to define what type each component is stored as
        typename T::component_type;

        // needs to say how many components
        { T::components };
    };

    struct format_rgba8
    {
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

            constexpr static colour_type hsv(float, float, float);

        private:
            constexpr colour_type() noexcept = default;

            typename Format::component_type component[Format::components];
    };

    using colour = colour_type<format_rgba8>;

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

    template <typename T>
    constexpr inline T cabs(const T& value) noexcept
    {
        if (value == 0.0)
            return value;

        return (T{} > value) ? -value : value;
    }

    constexpr inline double cmod(const double a, const double b) noexcept
    {
        return (a - (int(a / b) * b));
    }

    constexpr inline std::tuple<float, float, float> hsv_to_rgb(float hue, float saturation, float value)
    {
        float chroma = value * saturation;
        float intermediate = chroma * (1.0f - cabs(cmod(hue / 60.0, 2) -1.0f));
        float major = value - chroma;

        float red = 0.0;
        float green = 0.0;
        float blue = 0.0;

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

    constexpr inline std::tuple<float, float, float> rgb_to_hsv(float red, float green, float blue)
    {
        float hue = red;
        float saturation = green;
        float value = blue;

        return std::tie(hue, saturation, value);
    }

    template <ColourFormat Format>
    constexpr colour_type<Format> colour_type<Format>::hsv(float hue, float saturation, float value)
    {
        colour_type rval;
        const auto [red, green, blue] = hsv_to_rgb(hue, saturation, value);

        rval.component[Format::red_component] = 255 * red;
        rval.component[Format::green_component] = 255 * green;
        rval.component[Format::blue_component] = 255 * blue;
        rval.component[Format::alpha_component] = 255;

        return rval;
    }
}

#endif
