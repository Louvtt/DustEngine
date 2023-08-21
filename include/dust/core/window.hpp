#ifndef _DUST_CORE_WINDOW_HPP_
#define _DUST_CORE_WINDOW_HPP_

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "types.hpp"

namespace dust {

class Window 
{
private:
    GLFWwindow* m_window;

    u32 m_width;
    u32 m_height;

    static bool isWindowManagerInitialized;
public:
    /**
     * @brief Window creation flags
     */
    enum class Flags : int {
        Default    = 0x0,
        Borderless = 0x1,
        FullScreen = 0x2,
        Decorated  = 0x4,
        Maximize   = 0x8,
    };

    Window(const std::string& name, u32 width, u32 height, Flags flags = Flags::Default);
    ~Window();

    void flush();
    /**
     * @brief swap buffers
     * @see This is called in flush so use it independantly than Window::flush().
     */
    void swapBuffers();

    void setVSync(bool vsync);

    u32 getWidth() const;
    u32 getHeight() const;
    GLFWwindow* getNativeWindow() const;

    bool shouldClose() const;

private:
    void resize(u32 width, u32 height);
};

}

#endif //_DUST_CORE_WINDOW_HPP_