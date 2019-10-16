#ifndef UNORTHODOX_WINDOW
#define UNORTHODOX_WINDOW

#include <cstdint>
#include <experimental/propagate_const>
#include <memory>
#include <string>

#include "event.hpp"

namespace unorthodox
{
    using error_code = int;

    struct window_info
    {
        int8_t red_bits     = 8;
        int8_t green_bits   = 8;
        int8_t blue_bits    = 8;
        int8_t alpha_bits   = 8;
    };

    struct view_size
    {
        uint16_t width = 0;
        uint16_t height = 0;
    };

    struct empty {};
    struct empty2 {};

    template <typename T> concept any_renderer = requires(T t)
    {
        true;
    };

    template <typename T> concept video_target = requires(T t)
    {
        { t.set_title() };
        { t.set_size() };
        { t.is_open() };
    };

    template <typename Renderer, typename Target>
    class basic_video_output : public Renderer, public Target
    {
        public:
            basic_video_output(window_info = window_info());
           ~basic_video_output();

            basic_video_output&     set_title(std::string title);
            basic_video_output&     set_size(view_size);
            basic_video_output&     set_event_queue(event_queue&);

            error_code              create();

            basic_video_output(const basic_video_output&) = delete;
            basic_video_output(basic_video_output&&) = default;
    };

    using window = basic_video_output<empty, empty2>;
}

#endif
