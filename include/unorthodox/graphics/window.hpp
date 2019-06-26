#ifndef UNORTHODOX_GRAPHICS_WINDOW_HPP
#define UNORTHODOX_GRAPHICS_WINDOW_HPP

#include <unorthodox/video_io.hpp>

namespace unorthodox::context
{
    struct gles3 {};
}

namespace unorthodox
{
    struct Event
    {
    };

    class MinimalRenderer
    {
    };

    class MinimalTarget
    {
    };

    template <typename Context>
    class GenericOpenGLRenderer
    {
        public:
            using context_type = Context;

            template <typename T> requires unorthodox::is_colour<T>::value
            void clear(T value);

            // treat uint32_t as rgba hex
            void clear(uint32_t value) { clear(unorthodox::colour::rgba(value)); }
    };

    template <typename Context>
    template <typename T> requires unorthodox::is_colour<T>::value
    void GenericOpenGLRenderer<Context>::clear(T value)
    {
        if (value.as_rgba_int())
            return;
        return;
    }

    class GLFWWindowVideoOut
    {
        public:
           ~GLFWWindowVideoOut();

            constexpr static bool is_resizable = false;
            constexpr static bool is_headless = false;

            constexpr static bool sends_events = false;
            constexpr static bool receives_events = false;

            GLFWWindowVideoOut& set_title(std::string);
            GLFWWindowVideoOut& set_size(Size2D size);

            void create();
            bool is_open();

            void resize();

            void present();

            void wait_events();
            void poll_events();

            void add_callback_for_event(Event, std::function<void(const Event&)>);

        private:
            std::string current_title = "";
            Size2D      window_size;
    };

    using window = basic_video_output<GenericOpenGLRenderer<context::gles3>, GLFWWindowVideoOut>;
}

#endif
