#include "dust/core/window.hpp"

#include "dust/core/log.hpp"

#include "GLFW/glfw3.h"
#include <cstddef>

bool dust::Window::isWindowManagerInitialized = false;

dust::Window::Window(const string& name, u16 width, u16 height, Flags flags)
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
        DUST_ERROR("[GLFW] [{}] Failed to create the window {}", code, errorDescription);
    }
    glfwMakeContextCurrent(m_window);
}

dust::Window::~Window()
{
    glfwDestroyWindow(m_window);
    // terminate
    glfwTerminate();
    isWindowManagerInitialized = false;
}

void dust::Window::beginFrame()
{
    glfwPollEvents();
}
void dust::Window::endFrame()
{
    glfwSwapBuffers(m_window);
}

bool dust::Window::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}