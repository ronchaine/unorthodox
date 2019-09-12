#ifndef UNORTHODOX_COMMON_EVENT_SYSTEM
#define UNORTHODOX_COMMON_EVENT_SYSTEM

#include <chrono>
#include <vector>

#include <experimental/propagate_const>

#include <unorthodox/error_codes.hpp>

namespace unorthodox
{
    // Platform-specific, a file descriptor on POSIX?
    class event_source
    {
        public:
        private:
    };

    class event
    {
        public:
        private:
    };

    using event_list = std::vector<event>;

    class event_group
    {
        public:
            event_group();
            event_group(event_group&&);
            event_group(const event_group&) = delete;

            event_group& operator=(event_group&&);
            event_group& operator=(const event_group&) = delete;

            error_code add_source(event_source& src);
            error_code remove_source(event_source& src);

            event_list wait(const std::chrono::milliseconds timeout = std::chrono::milliseconds(0));
            event_list poll();

        private:
            struct impl;
            std::experimental::propagate_const<std::unique_ptr<impl>> implementation;
    };
}

#endif
