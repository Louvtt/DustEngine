#ifndef _DUST_CORE_WINDOW_HPP_
#define _DUST_CORE_WINDOW_HPP_

#include "GLFW/glfw3.h"
#include "types.hpp"

namespace dust {

class Window 
{
private:
    GLFWwindow* m_window;

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
    };

    Window(const string& name, u16 width, u16 height, Flags flags = Flags::Default);
    ~Window();

    void beginFrame();
    void endFrame();

    bool shouldClose() const;
};

}

#endif //_DUST_CORE_WINDOW_HPP_