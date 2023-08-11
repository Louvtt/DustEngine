#include "dust/core/window.hpp"

#include "dust/core/log.hpp"

#include "GLFW/glfw3.h"
#include <cstddef>

bool dust::Window::isWindowManagerInitialized = false;

static void error_callback(int error, const char* description)
{
    DUST_ERROR("[GLFW] [{}] {}\n", error, description);
}


dust::Window::Window(const std::string& name, u32 width, u32 height, Flags flags)
: m_width(width),
m_height(height)
{
    // glfw initialisation
    if(!isWindowManagerInitialized) {
        if(!glfwInit()) {
            const char* errorDescription;
            int code = glfwGetError(&errorDescription);
            DUST_ERROR("[GLFW] [{}] Failed to initialize GLFW {}", code, errorDescription);
            return;
        }
        isWindowManagerInitialized = true;
        DUST_INFO("[GLFW] Initialisation successfull.");
    }

    // error callback
    glfwSetErrorCallback(error_callback);

    // create window
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        #ifndef __MACOSX__
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // render doc need it
        #endif
        
        m_window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
        if(m_window == nullptr) {
            const char* errorDescription;
            int code = glfwGetError(&errorDescription);
            DUST_ERROR("[GLFW] [{}] Failed to create the window {}", code, errorDescription);
        }
        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(1); // TODO: Vsync option
    }
}

dust::Window::~Window()
{
    glfwMakeContextCurrent(nullptr);
    glfwDestroyWindow(m_window);
    DUST_INFO("[GLFW] Terminating glfw.");
    // terminate
    glfwTerminate();
    isWindowManagerInitialized = false;
}

void dust::Window::flush()
{
    glfwPollEvents();
    glfwSwapBuffers(m_window);
}

bool dust::Window::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

u32 dust::Window::getHeight() const
{
    return m_height;
}

u32 dust::Window::getWidth() const
{
    return m_width;
}