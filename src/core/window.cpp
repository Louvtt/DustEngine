#include "mx/core/window.hpp"

#include "mx/core/log.hpp"

#include "GLFW/glfw3.h"
#include <cstddef>

bool mx::Window::isWindowManagerInitialized = false;

mx::Window::Window(const string& name, u16 width, u16 height, Flags flags)
{
    // glfw initialisation
    if(!isWindowManagerInitialized) {
        if(!glfwInit()) {
            const char* errorDescription;
            int code = glfwGetError(&errorDescription);
            MX_ERROR("[GLFW] [{}] Failed to initialize GLFW {}", code, errorDescription);
            return;
        }
        isWindowManagerInitialized = true;
        MX_INFO("[GLFW] Initialisation successfull.");
    }

    // create window
    glfwWindowHint(GLFW_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_VERSION_MINOR, 6);
    #ifndef __MACOSX__
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // render doc need it
    #endif
    m_window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if(m_window == nullptr) {
        const char* errorDescription;
        int code = glfwGetError(&errorDescription);
        MX_ERROR("[GLFW] [{}] Failed to create the window {}", code, errorDescription);
    }
    glfwMakeContextCurrent(m_window);
}

mx::Window::~Window()
{
    glfwDestroyWindow(m_window);
    // terminate
    glfwTerminate();
    isWindowManagerInitialized = false;
}

void mx::Window::beginFrame()
{
    glfwPollEvents();
}
void mx::Window::endFrame()
{
    glfwSwapBuffers(m_window);
}

bool mx::Window::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}