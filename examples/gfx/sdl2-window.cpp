#define UNORTHODOX_ENABLE_MACROS

#include <unorthodox/graphics/canvas.hpp>
#include <unorthodox/graphics/renderer.hpp>

#include <cppevents/window.hpp>
#include <cppevents/input.hpp>

#include <memory>

#include "opengl-es-renderer.hpp"

#include "sdl2-window.hpp"

namespace cppevents::detail { cppevents::event create_sdl_window_event(native_source_type fd); }

namespace unorthodox
{
    sdl2_window::sdl2_window() noexcept
    {
        SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
        if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
            std::abort();
    }

    sdl2_window::~sdl2_window()
    {
        if (window)
            SDL_DestroyWindow(window);

        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    sdl2_window& sdl2_window::set_title(std::string new_title)
    {
        title = std::move(new_title);
        if (window != nullptr)
            SDL_SetWindowTitle(window, title.c_str());

        return *this;
    }

    sdl2_window& sdl2_window::set_size(unorthodox::size_2D new_size)
    {
        size = std::move(new_size);
        if (window != nullptr)
            SDL_SetWindowSize(window, size.w, size.h);

        return *this;
    }

    void sdl2_window::create(const unorthodox::window_configuration conf)
    {
        (void)conf;
        //TODO: setup by conf params
//        renderer = std::make_unique<opengl_renderer>(conf);
//        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

//        fmt::print(stderr, "creating window of size {}x{}, title: {}\n", size.w, size.h, title);

        window = SDL_CreateWindow(title.c_str(),
                                  0,
                                  0,
                                  size.w, size.h,
                                  SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

        if (!window)
            std::cerr << "sdl window creation failed\n";

        context = SDL_GL_CreateContext(window);
/*
        if (context == 0)
            fmt::print(stderr, "couldn't create GL context\n");
*/
        SDL_GL_MakeCurrent(window, context);


/*
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);

        fmt::print(stderr, "libepoxy + SDL init, GL version: {}\n", epoxy_gl_version());
*/
        present();
    }

    bool sdl2_window::is_open() const noexcept
    {
        return opened;
    }

    bool sdl2_window::is_dirty() const noexcept
    {
        return dirty;
    }

    void sdl2_window::redraw() noexcept
    {
        dirty = true;
    }

    void sdl2_window::present() noexcept
    {
        if (!dirty)
            return;

        /*
        if (renderer == nullptr)
        {
            fmt::print(stderr, "cannot present window before renderer init\n");
            return;
        }
        renderer->flush();
        */
        SDL_GL_SwapWindow(window);
        dirty = false;
    }

    void sdl2_window::wait()
    {
        SDL_Event event;
        SDL_WaitEvent(&event);
    }

    void sdl2_window::poll()
    {
        SDL_Event event;
        SDL_PollEvent(&event);
    }

    void sdl2_window::connect_events(cppevents::event_queue& queue, int state)
    {
        (void)state;

        assert(window != nullptr);

        cppevents::native_source_type src = cppevents::get_sdl_event_source(window);
        queue.add_native_source(src, cppevents::detail::create_sdl_window_event);
    }
}

