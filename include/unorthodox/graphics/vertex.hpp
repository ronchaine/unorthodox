#ifndef UNORTHODOX_VERTEX_HPP
#define UNORTHODOX_VERTEX_HPP

#include <cstdint>
#include <vector>

#include "../buffer.hpp"

namespace unorthodox::detail
{
    template <typename T, uint32_t Pos_components> struct vertex_position_component; // { std::array<T, Pos_components> coord; };

    template <typename T> struct vertex_position_component<T,0> {};
    template <typename T> struct vertex_position_component<T,1> { T x; };
    template <typename T> struct vertex_position_component<T,2> { T x, y; };
    template <typename T> struct vertex_position_component<T,3> { T x, y, z; };
    template <typename T> struct vertex_position_component<T,4> { T x, y, z, w; };

    template <uint32_t UV_count> struct vertex_texture_coordinate_component
    {
        uint16_t u[UV_count];
        uint16_t v[UV_count];
    };
    template <> struct vertex_texture_coordinate_component<1>
    {
        uint16_t u;
        uint16_t v;
    };
    template <> struct vertex_texture_coordinate_component<0> {};

    struct vertex_rgba_colour_component
    {
        uint8_t r = 0xff;
        uint8_t g = 0xff;
        uint8_t b = 0xff;
        uint8_t a = 0xff;

        void set_colour(uint32_t rgba)
        {
            r = rgba >> 24;
            g = (0x00ff0000 & rgba) >> 16;
            b = (0x0000ff00 & rgba) >> 8;
            a = 0xff & rgba;
        }
    };

    struct vertex_hsv_shift_component
    {
        float hue           = 0.0f;
        float saturation    = 0.0f;
        float value         = 0.0f;
    };

    template <typename T> struct vertex_empty_component {};

    template <uint32_t Flags> struct vertex_extra_components {};
}

namespace unorthodox
{
    constexpr static uint32_t VERTEX_RGBA_COLOUR    = 0x01;
    constexpr static uint32_t VERTEX_HSV_SHIFT      = 0x02;

    constexpr static uint32_t VERTEX_1D             = 2;
    constexpr static uint32_t VERTEX_2D             = 2;
    constexpr static uint32_t VERTEX_3D             = 3;

    constexpr static uint32_t NO_TEXTURES           = 0;
}

namespace unorthodox
{
    template <typename T> concept vertex_type = requires(T t)
    {
        // needs to know what type dimension components are
        typename T::component_type;

        // how many position components
        { T::dim };

        // how many texture coordinate pairs
        { T::uv_count };

        // extra features defined by this header
        { T::feature_flags };
    };
}

namespace unorthodox
{
    template <typename T, uint32_t Pos_count, uint32_t UV_count, uint32_t Flags = VERTEX_RGBA_COLOUR>
    struct vertex : public detail::vertex_position_component<float, Pos_count>,
                    public detail::vertex_texture_coordinate_component<UV_count>,
                    public detail::vertex_extra_components<Flags>
    {
        constexpr static uint32_t rgba_colour   = 0x01;

        constexpr static uint32_t dim           = Pos_count;
        constexpr static uint32_t uv_count      = UV_count;
        constexpr static uint32_t feature_flags = Flags;
    };

    class vertex_array
    {
        public:
        private:
            /*
            uint32_t dim = 0;
            uint32_t uv_count = 0;
            uint32_t feature_flags = 0;
            */
            buffer vertex_data;
    };
}

#endif
