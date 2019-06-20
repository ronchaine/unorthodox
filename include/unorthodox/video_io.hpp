#ifndef UNORTHODOX_GRAPHICS_IO_HPP
#define UNORTHODOX_GRAPHICS_IO_HPP

#include <experimental/propagate_const>
#include <memory>

#include <unorthodox/colour.hpp>

namespace unorthodox
{
    template <typename T> concept Renderer = requires(T t)
    {
        typename T::context_type;

        { t.clear };
    };
    
    template <typename T> concept VideoTarget = requires(T t)
    {
        // Target traits
        { T::is_resizable } -> const bool;
        { T::is_headless } -> const bool;
        { T::sends_events } -> const bool;
        { T::receives_events } -> const bool;

        { t.present() };

        requires (T::is_resizable == false) || requires { t.resize(); };
        requires (T::is_resizable == false) || requires { t.set_size(); };
        requires (T::is_headless == true) || requires {{ t.is_open() } -> bool; };

//        requires (T::sends_events == false); || requires {};
//        requires (T::receives_events == false) || requires {{ t.is_open() } -> bool; };
    };

    struct context_info
    {
    };

    struct window_info
    {
        struct colour_info_type
        {
            int8_t red_bits     = 8;
            int8_t green_bits   = 8;
            int8_t blue_bits    = 8;
            int8_t alpha_bits   = 8;
        };

        constexpr window_info() = default;

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
        if (info.colour_info.red_bits == 8)
            return;
        return;
    }

    template <Renderer R, VideoTarget T>
    basic_video_output<R, T>::~basic_video_output()
    {
    }
}

#endif
