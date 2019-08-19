#ifndef UNORTHODOX_EVENT_HPP
#define UNORTHODOX_EVENT_HPP

using event_desc = uint16_t;

// Event sources
constexpr static event_desc src_keyboard    = 0x0001;
constexpr static event_desc src_mouse       = 0x0002;
constexpr static event_desc src_touch       = 0x0004;
constexpr static event_desc src_stylus      = 0x0008;
constexpr static event_desc src_gamepad     = 0x0010;
constexpr static event_desc src_window      = 0x0020;
constexpr static event_desc src_timer       = 0x0040;
constexpr static event_desc src_network     = 0x0080;
constexpr static event_desc src_filesystem  = 0x0100;
constexpr static event_desc src_variable    = 0x0200;

constexpr static event_desc src_any         = 0xffff;

// Event types
constexpr static event_desc type_press      = 0x0001;
constexpr static event_desc type_release    = 0x0002;
constexpr static event_desc type_motion     = 0x0004;
constexpr static event_desc type_gesture    = 0x0008;

template <typename T>
concept event_type = requires(T t)
{
}

class keyboard_event
{
    using callback_fun_type = std::function<void(keyboard_event)>;
};

describe_event(src_keyboard | src_mouse);

on_event(Event& ev).call([&](auto& event){
        });

#endif
