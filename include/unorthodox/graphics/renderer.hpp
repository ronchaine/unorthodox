#ifndef UNORTHODOX_GRAPHICS_RENDERER
#define UNORTHODOX_GRAPHICS_RENDERER

#include "vertex.hpp"
#include "../colour.hpp"

namespace unorthodox {
    class renderer
    {
        public:
            virtual void queue(const vertex_array& array) = 0;
            virtual void clear(colour) = 0;

            virtual void flush() = 0;

            virtual ~renderer() = default;
    };
}

#endif
