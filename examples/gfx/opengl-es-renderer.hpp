#include <unorthodox/graphics/canvas.hpp>
#include <unorthodox/graphics/vertex.hpp>
#include <unorthodox/colour.hpp>

namespace unorthodox::graphics
{
    class opengl_renderer
    {
        public:
            constexpr static bool requires_opengl_support = true;

            opengl_renderer() = default;
            opengl_renderer(const window_configuration&);

            void queue(const vertex_array&);
            void clear(const colour);

            void load_shader(std::string_view vertex_filename, std::string_view fragment_filename);

            void flush();
    };
}
