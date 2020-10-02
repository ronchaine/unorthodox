#ifndef SDL2_WINDOW_HEADER
#define SDL2_WINDOW_HEADER

#include <unorthodox/graphics/canvas.hpp>
#include <cppevents/sdl2.hpp>

namespace unorthodox::graphics
{
    class sdl2_window
    {
        public:
            constexpr static bool supports_opengl = true;
            constexpr static bool enable_cppevents = true;

            sdl2_window() noexcept;
            ~sdl2_window();

            sdl2_window& set_title(std::string);
            sdl2_window& set_size(unorthodox::size_2D);

            void create(const window_configuration = window_configuration{});

            void redraw() noexcept;
            void present() noexcept;

            void wait();
            void poll();

            void connect_events(cppevents::event_queue& queue, int state);

            bool is_open() const noexcept;
            bool is_dirty() const noexcept;

        private:
            bool opened = true;
            bool dirty = true;

            SDL_Window* window = nullptr;
            SDL_GLContext context;

            size_2D size;
            std::string title;
    };
}

namespace cppevents
{
    template <typename Source, ::unorthodox::graphics::suitable_canvas T>
        requires std::is_same_v<Source, window_event> &&
                 std::is_convertible_v<T, unorthodox::graphics::sdl2_window>
    error_code add_source(T& src, event_queue& q = default_queue)
    {
        src.connect_events(q, 0);
        return error_code::success;
    }
}
#endif
