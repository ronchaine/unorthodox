#ifndef UNORTHODOX_GRAPHICS_UTIL_HPP
#define UNORTHODOX_GRAPHICS_UTIL_HPP

#include "vertex.hpp"
#include "../colour.hpp"
#include "../geometry/shapes.hpp"

namespace unorthodox
{
    // TODO: constexpr this when vertex_array can be made constexpr
    template <typename T>
    vertex_array create_vertices(const T& shape) requires geometry::trivially_triangularizable<T>
    {
        (void)shape;
        vertex_array rval;

        /*
        rval.pos_components = T::dimension;
        rval.uv_components = 0;
        */
        return rval;
    }

    template <typename T>
    vertex_array paint(const T& thing, uint32_t int_colour)
    {
        return paint(thing, colour_type<format_rgba8>::rgba(int_colour));
    }

    template <typename T, ColourFormat CType>
    vertex_array paint(const T& shape, colour_type<CType> col)
    {
        (void)col;
        return create_vertices(shape);
    }
}

#endif
