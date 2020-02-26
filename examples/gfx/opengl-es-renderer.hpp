#include <unorthodox/graphics/canvas.hpp>
#include <unorthodox/graphics/renderer.hpp>

namespace unorthodox
{
    class opengl_renderer
    {
        public:
            opengl_renderer() = default;
            opengl_renderer(const unorthodox::window_configuration&);

            void queue(const unorthodox::vertex_array&);
            void clear(const unorthodox::colour);

            void load_shader(std::string_view vertex_filename, std::string_view fragment_filename);

            void flush();
    };
}
