#ifndef UNORTHODOX_GRAPHICS_IO_HPP
#define UNORTHODOX_GRAPHICS_IO_HPP

#include <experimental/propagate_const>
#include <memory>

// -std=c++2a -Xclang -fconcepts-ts

// #include "../include/unorthodox/colour.hpp"
#include "colour.hpp"

namespace unorthodox
{
    template <typename T> concept Renderer = requires(T t)
    {
        { t.clear };
    };
    
    template <typename T> concept VideoOutput = requires(T t)
    {
        { t.present() };
    };

    struct window_info
    {
        enum class context_type
        {
            NO_CONTEXT,
            OPENGL_ES_CONTEXT,
            OPENGL_CONTEXT,
        };

        struct colour_bits
        {
            int8_t red_bits     = 8;
            int8_t green_bits   = 8;
            int8_t blue_bits    = 8;
            int8_t alpha_bits   = 8;
        };

        constexpr window_info() = default;

        context_type    context = context_type::OPENGL_ES_CONTEXT;
        colour_bits     colour;
    };

    template <Renderer RenderingType, VideoOutput Output>
    class basic_video_output : public RenderingType, public Output
    {
        public:
            basic_video_output(const window_info = window_info());
           ~basic_video_output();

        private:
    };

    template <Renderer R, VideoOutput O>
    basic_video_output<R, O>::basic_video_output(const window_info info)
    {
    }

    template <Renderer R, VideoOutput O>
    basic_video_output<R, O>::~basic_video_output()
    {
    }

//    using window = basic_video_output;
}

#endif
