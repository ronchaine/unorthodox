#include "window.hpp"
#include "event.hpp"

int main()
{
    unorthodox::window window;
    unorthodox::event_queue events;

    window.set_title("Unorthodox graphics test")
          .set_size({1270, 720})
          .set_event_queue(events)
          .create();
}

