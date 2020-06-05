#include <unorthodox/geometry/shapes.hpp>
#include <unorthodox/graphics/canvas.hpp>
#include <unorthodox/graphics/vertex.hpp>

#include "gfx/sdl2-window.hpp"
#include "gfx/opengl-es-renderer.hpp"

namespace unorthodox::geometry
{
    // constexpr this when possible
    template <typename T>
    graphics::vertex_array paint(const T&, colour) requires geometry::trivially_triangularizable<T>
    {

        //using vertex_type = unorthodox::graphics::vertex<unorthodox::graphics::vertex_components::position<typename T::value_type, T::dimension>>;

        using vertex_type = unorthodox::graphics::vertex<
            unorthodox::graphics::vertex_components::position<typename T::value_type, T::dimension>,
            unorthodox::graphics::vertex_components::texture_coordinates<1>
        >;

        graphics::vertex_array rval;

        vertex_type v0;
        v0.x = 1;
        v0.y = 2;

        rval.add_vertices(v0);
//        auto batch = vertex_batch;

        return rval;
    }
}

int main()
{
    // Create video output, we want to draw to an SDL2 window, and use OpenGL to do that
    unorthodox::graphics::canvas<unorthodox::graphics::sdl2_window, unorthodox::graphics::opengl_renderer> window;

    window.set_title("Unorthodox graphics test")
          .set_size({1280, 720})
          .create();

    // describe a shape that can be used in rendering.  By default viewport coordinates equal pixel coordinates
    unorthodox::geometry::triangle triangle({100,200}, {200, 200}, {150, 100});

    // clear back buffer to colour specified
    window.clear(0xFF00FFFF);

    // queue triangle to be drawn
    paint(triangle, 0xffffffff);

    //window << paint(unorthodox::Something{}, 0xffffffff);

//    window << paint(triangle, texture, 0xffffffff);

//    Easy plots!
//    window << plot({array, array});

//    Text
//    window << "String literals work!";
//    window << paint("You can also paint strings!", 0xff0000ff);

    /*
    window.clear(0x300000a0);
    */
/*

    cppevents::on_event<cppevents::mouse_button>([&](cppevents::event& raw){
        auto ev = event_cast<cppevents::mouse_button>(raw);
        std::cout << "Button " << static_cast<uint32_t>(ev.button) << " ";
        if (ev.type == cppevents::mouse_button::button_down)
            std::cout << "pressed ";
        else
            std::cout << "released ";
    });
*/
    window.present();

    cppevents::add_source<cppevents::window>(window);

    while (window.is_open())
    {
        // block until an event is received
        cppevents::wait();

        window.clear(0xFF00FFFF);

        // flush window drawing operations and swap back and front buffers
        // (show what has been queued to be drawn)
        window.present();
    }
}
