#ifndef UNORTHODOX_BASIC_TYPES_HPP
#define UNORTHODOX_BASIC_TYPES_HPP

#include <cstdint>

namespace unorthodox
{
    struct Size2D
    {
        union {
            int32_t w = 0;
            int32_t width;
        };
        union {
            int32_t h = 0;;
            int32_t height;
        };
    };
}

#endif
