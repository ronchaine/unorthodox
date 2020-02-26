#ifndef UNORTHODOX_GRAPHICS_CANVAS_HPP
#define UNORTHODOX_GRAPHICS_CANVAS_HPP

#include "../basic_types.hpp"

namespace unorthodox
{
    struct window_configuration{};

    template <typename T> concept suitable_renderer = requires(T t)
    {
        true;
    };

    template <typename T> concept suitable_canvas = requires(T t)
    {
        true;
    };

    template <suitable_canvas Target, suitable_renderer Renderer>
    class canvas : public Target, public Renderer {};

}

#endif
