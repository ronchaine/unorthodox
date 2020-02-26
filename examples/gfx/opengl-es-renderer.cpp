#include <epoxy/gl.h>

#include "opengl-es-renderer.hpp"

namespace unorthodox
{
    opengl_renderer::opengl_renderer(const unorthodox::window_configuration& conf)
    {
        (void)conf;
    }

    void opengl_renderer::queue(const unorthodox::vertex_array& array)
    {
        (void)array;
    }

    void opengl_renderer::clear(const unorthodox::colour col)
    {
        glClearColor(col.red() / 255.0, col.green() / 255.0, col.blue() / 255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void opengl_renderer::flush()
    {
    }

    void opengl_renderer::load_shader(std::string_view vertex_filename, std::string_view fragment_filename)
    {
        (void)vertex_filename;
        (void)fragment_filename;
    }
}
