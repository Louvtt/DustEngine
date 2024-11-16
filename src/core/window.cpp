#include "dust/core/window.hpp"
#include "dust/core/profiling.hpp"

#include "dust/core/application.hpp"
#include "dust/core/log.hpp"

#include "backends/imgui_impl_glfw.h"

#include "GLFW/glfw3.h"
#include <type_traits>

bool dust::Window::isWindowManagerInitialized = false;

static void error_callback(int error, const char* description)
{
    DUST_ERROR("[GLFW] [{}] {}\n", error, description);
}


dust::Window::Window(const std::string& name, u32 width, u32 height, Flags flags)
: m_width(width),
m_height(height)
{
    DUST_PROFILE_SECTION("Window::Constructor");
    // glfw initialisation
    if(!isWindowManagerInitialized) {
        DUST_PROFILE_SECTION("GLFW Init");
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
        DUST_PROFILE_SECTION("Window::GLFWwindow Creation");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // render doc need it
        
        m_window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
        if(m_window == nullptr) {
            const char* errorDescription;
            int code = glfwGetError(&errorDescription);
            DUST_ERROR("[GLFW] [{}] Failed to create the window {}", code, errorDescription);
        }
        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(1); // TODO: Vsync option

        // glfw event bindings to event system
        glfwSetFramebufferSizeCallback(m_window, 
        [](GLFWwindow* _window, int w, int h) -> void {
            auto window = Application::Get()->getWindow();
            window->resize(w, h);
            auto renderer = Application::Get()->getRenderer();
            renderer->resize(w, h);
        });
    }

}

dust::Window::~Window()
{
    DUST_PROFILE;
    glfwMakeContextCurrent(nullptr);
    glfwDestroyWindow(m_window);
    DUST_INFO("[GLFW] Terminating glfw.");
    // terminate
    glfwTerminate();
    isWindowManagerInitialized = false;
}

void dust::Window::flush()
{
    DUST_PROFILE_SECTION("Window::flush");
    glfwPollEvents();
    swapBuffers();
}

void dust::Window::swapBuffers() 
{
    glfwSwapBuffers(m_window);
    DUST_PROFILE_GPU_COLLECT;
}

void dust::Window::setVSync(bool vsync)
{
    // glfwMakeContextCurrent(m_window);
    glfwSwapInterval(vsync?1:0);
}

bool dust::Window::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

u32 dust::Window::getHeight() const
{
    return m_height;
}

GLFWwindow* dust::Window::getNativeWindow() const
{
    return m_window;
}

u32 dust::Window::getWidth() const
{
    return m_width;
}

void dust::Window::resize(u32 width, u32 height)
{
    m_width = width;
    m_height = height;
}