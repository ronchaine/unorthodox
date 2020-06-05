#ifndef UNORTHODOX_GRAPHICS_CANVAS_HPP
#define UNORTHODOX_GRAPHICS_CANVAS_HPP

#include "../basic_types.hpp"

namespace unorthodox::graphics
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
    class canvas : public Target, public Renderer {
        static_assert(not Renderer::requires_opengl_support or Target::supports_opengl,
                "Renderer requires OpenGL but target does not support it");
    };

}

#endif
