#include <unorthodox/basic_types.hpp>
#include <unorthodox/graphics/window.hpp>

#include <GLFW/glfw3.h>

#include <iostream>

namespace unorthodox
{
    static GLFWwindow* window_pointer = nullptr;

    GLFWWindowVideoOut::~GLFWWindowVideoOut()
    {
        glfwTerminate();
    }

    GLFWWindowVideoOut& GLFWWindowVideoOut::set_title(std::string new_title)
    {
        if (window_pointer != nullptr)
            glfwSetWindowTitle(window_pointer, new_title.c_str());

        current_title = std::move(new_title);
        return *this;
    }

    GLFWWindowVideoOut& GLFWWindowVideoOut::set_size(Size2D size)
    {
        if (window_pointer != nullptr)
            glfwSetWindowSize(window_pointer, size.w, size.h);

        window_size = size;

        return *this;
    }

    void GLFWWindowVideoOut::create()
    {
        if (window_pointer != nullptr)
            return;

        glfwInit();
        window_pointer = glfwCreateWindow(window_size.w, window_size.h, current_title.c_str(), nullptr, nullptr);
    }

    bool GLFWWindowVideoOut::is_open()
    {
        if (window_pointer == nullptr)
            return false;

        return !glfwWindowShouldClose(window_pointer);
    }

    void GLFWWindowVideoOut::resize()
    {
    }

    void GLFWWindowVideoOut::present()
    {
    }

    void GLFWWindowVideoOut::wait_events()
    {
    }

    void GLFWWindowVideoOut::poll_events()
    {
    }

    void GLFWWindowVideoOut::add_callback_for_event(Event, std::function<void(const Event &)>)
    {
    }
}
