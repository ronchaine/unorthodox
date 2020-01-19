#ifndef UNORTHODOX_GRAPHICS_UTIL_HPP
#define UNORTHODOX_GRAPHICS_UTIL_HPP

#include "vertex.hpp"
#include "../colour.hpp"

namespace unorthodox
{
    template <typename T>
    vertex_array paint(const T& thing, uint32_t int_colour)
    {
        return paint(thing, colour_type<format_rgba8>::rgba(int_colour));
    }

    template <typename T, ColourFormat CType>
    vertex_array paint(const T&, colour_type<CType> col)
    {
        (void)col;
        return vertex_array{};
    }
}

#endif
