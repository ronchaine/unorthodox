#ifndef UNORTHODOX_WINDOW_HPP
#define UNORTHODOX_WINDOW_HPP

#include <experimental/propagate_const>

#include "basic_types.hpp"

#if defined(HAS_CPPEVENTS)
#include <cppevents/window.hpp>
#include <cppevents/input.hpp>
#endif

namespace unorthodox
{
    enum class pixel_unit_type
    {
        FLOATING_POINT,
        SIGNED_INTEGER,
        UNSIGNED_INTEGER,
    };

    enum class context_type
    {
        NONE,
        OPENGL_ES,
        OPENGL_CORE,
        OPENGL_COMPATIBILITY,
        VULKAN,
    };

    struct window_colour_info
    {
        int red_bits    = 8;
        int green_bits  = 8;
        int blue_bits   = 8;
        int alpha_bits  = 8;
    };

    struct window_buffer_info
    {
        int accum_red_bits      = 0;
        int accum_green_bits    = 0;
        int accum_blue_bits     = 0;
        int accum_alpha_bits    = 0;

        int depth_bits          = 16;
        int stencil_bits        = 0;
    };

    struct window_context_info
    {
        context_type context;
    };

    struct window_configuration
    {
        bool allow_resize = true;
    };

    static inline window_configuration default_window_configuration = window_configuration();

    template <typename Backend>
    class window
    {
        public:
            window();
            ~window();

            void create(window_configuration& conf = default_window_configuration);

            window& set_title(const std::string&);
            window& set_size(Size2D size);
            
            void present() noexcept;

            void wait();
            void poll();

            bool is_open() const noexcept;
            bool requires_redraw() const noexcept;

            #if defined(HAS_CPPEVENTS)
            void connect_events(cppevents::event_queue& queue, int state);
            #endif

        private:
            std::experimental::propagate_const<std::unique_ptr<Backend>> impl;
    };
}

#ifdef UNORTHODOX_ENABLE_MACROS
#ifdef HAS_CPPEVENTS
#define IMPLEMENT_WINDOW_BACKEND(x) \
namespace unorthodox \
{ \
    template <> ::unorthodox::window<x>::window() : impl(std::make_unique<x>()) {} \
    template <> ::unorthodox::window<x>::~window() = default; \
\
    template <> ::unorthodox::window<x>& \
    ::unorthodox::window<x>::set_title(const std::string& title) { impl->set_title(title); return *this; } \
\
    template <> ::unorthodox::window<x>& \
    ::unorthodox::window<x>::set_size(Size2D size) { impl->set_size(size); return *this; } \
\
    template <> void \
    ::unorthodox::window<x>::create(window_configuration& conf) { impl->create(conf); } \
\
    template <> void \
    ::unorthodox::window<x>::present() noexcept { impl->present(); } \
\
    template <> bool \
    ::unorthodox::window<x>::is_open() const noexcept { return impl->is_open(); } \
\
    template <> void \
    ::unorthodox::window<x>::wait() { impl->wait(); } \
\
    template <> void \
    ::unorthodox::window<x>::poll() { impl->poll(); } \
\
    template <> void \
    ::unorthodox::window<x>::connect_events(cppevents::event_queue& queue, int state) { return impl->connect_events(queue, state); } \
\
    template <> bool \
    ::unorthodox::window<x>::requires_redraw() const noexcept { return impl->is_dirty(); } \
}

#else
#error Macro not implemented yet
#endif

#endif

#endif
