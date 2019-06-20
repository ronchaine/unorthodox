#include <unorthodox/video_io.hpp>
#include <unorthodox/basic_types.hpp>

#include <unorthodox/graphics/window.hpp>

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

    class GLFWWindowVideoOut
    {
        public:
            constexpr static bool is_resizable = false;
            constexpr static bool is_headless = false;

            constexpr static bool sends_events = false;
            constexpr static bool receives_events = false;

            GLFWWindowVideoOut& set_title(std::string_view);
            GLFWWindowVideoOut& set_size(Size2D size);

            void create();
            bool is_open();

            void resize();

            void present();

            void wait_events();
            void poll_events();

            void add_callback_for_event(Event, std::function<void(const Event&)>);
    };

    using window = basic_video_output<GenericOpenGLRenderer<context::gles3>, GLFWWindowVideoOut>;
}

//int main(int argc, char* argv[])
int main()
{
    // Create default video output
    unorthodox::window window;

    window.set_title("BSDL test")
          .set_size({1280, 720})
          .create();

    // describe a shape that can be used in rendering.  By default viewport coordinates equal pixel coordinates
//    unorthodox::shapes::triangle triangle({100,200}, {200, 200}, {150, 100});

    // queue triangle to be drawn
//    window << paint(triangle, 0xffffffff);
//    window << paint(triangle, texture, 0xffffffff);

    while (window.is_open())
    {
        // clear back buffer to colour specified, no-op if not using a renderer
        window.clear(0x300000a0);

        // block until an event is received
        window.wait_events();

        // go through events, print their debug info
        /*
        for (auto& ev : window.events())
            ev.print_debug();
        */

        // flush window drawing operations and swap back and front buffers
        // (show what has been queued to be drawn)
        window.present();
    }
}
