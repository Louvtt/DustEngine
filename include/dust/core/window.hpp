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

    u32 getWidth() const;
    u32 getHeight() const;

    bool shouldClose() const;
};

}

#endif //_DUST_CORE_WINDOW_HPP_