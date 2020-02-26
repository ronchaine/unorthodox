#include <unorthodox/geometry/shapes.hpp>
#include <unorthodox/graphics/canvas.hpp>

#include "gfx/sdl2-window.hpp"
#include "gfx/opengl-es-renderer.hpp"

int main()
{
    // Create video output, we want to draw to an SDL2 window, and use OpenGL to do that
    unorthodox::canvas<unorthodox::sdl2_window, unorthodox::opengl_renderer> window;

    window.set_title("Unorthodox graphics test")
          .set_size({1280, 720})
          .create();

    // describe a shape that can be used in rendering.  By default viewport coordinates equal pixel coordinates
//    unorthodox::shapes::triangle triangle({100,200}, {200, 200}, {150, 100});

    // queue triangle to be drawn
//    window << paint(triangle, 0xffffffff);
//    window << paint(triangle, texture, 0xffffffff);

//    Easy plots!
//    window << plot({array, array});

//    Text
//    window << "String literals work!";
//    window << paint("You can also paint strings!", 0xff0000ff);

    /*
    window.clear(0x300000a0);
    window.present();
    */

    while (window.is_open())
    {
        // clear back buffer to colour specified, no-op if not using a renderer
        window.clear(0x300000a0);

        // block until an event is received
        cppevents::wait();

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
