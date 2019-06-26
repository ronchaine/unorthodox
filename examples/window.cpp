#include <unorthodox/video_io.hpp>
#include <unorthodox/basic_types.hpp>

#include <unorthodox/graphics/window.hpp>

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
