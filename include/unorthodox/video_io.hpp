#ifndef UNORTHODOX_GRAPHICS_IO_HPP
#define UNORTHODOX_GRAPHICS_IO_HPP

#include <experimental/propagate_const>
#include <memory>

#include <unorthodox/colour.hpp>

namespace unorthodox
{
    template <typename T> concept Renderer = requires(T t)
    {
        { t.clear };
    };
    
    template <typename T> concept VideoTarget = requires(T t)
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

        struct colour_info_type
        {
            int8_t red_bits     = 8;
            int8_t green_bits   = 8;
            int8_t blue_bits    = 8;
            int8_t alpha_bits   = 8;
        };

        constexpr window_info() = default;

        context_type        context = context_type::OPENGL_ES_CONTEXT;
        colour_info_type    colour_info;
    };

    template <Renderer RenderingType, VideoTarget Target>
    class basic_video_output : public RenderingType, public Target
    {
        public:
            basic_video_output(const window_info = window_info());
           ~basic_video_output();

        private:
    };

    template <Renderer R, VideoTarget T>
    basic_video_output<R, T>::basic_video_output(const window_info info)
    {
    }

    template <Renderer R, VideoTarget T>
    basic_video_output<R, T>::~basic_video_output()
    {
    }

//    using window = basic_video_output;
}

#endif
