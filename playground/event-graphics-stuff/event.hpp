#ifndef UNORTHODOX_EVENT
#define UNORTHODOX_EVENT

#include <cstdint>
namespace unorthodox
{
    class event_queue
    {
        public:
            enum event_types
            {
                NONE        = 0,

                // Window system events
                WINDOW      = 0x0001,

                // Input events
                KEYBOARD    = 0x0002,
                GAMEPAD     = 0x0004,
                TABLET      = 0x0008,
                MOUSE       = 0x0010,
                TOUCH       = 0x0020,

                // OS events
                FILESYSTEM  = 0x0040,
                NETWORK     = 0x0080,
                TIMER       = 0x0100,
                SIGNAL      = 0x0200,
                PROCESS     = 0x0400,
                NOTIFY      = 0x0800,

                ALL_EVENTS  = 0xffff
            };

        private:
    };
}

#endif
