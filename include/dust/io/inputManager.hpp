#ifndef _DUST_IO_INPUTMANAGER_HPP_
#define _DUST_IO_INPUTMANAGER_HPP_

#include "dust/core/types.hpp"
#include "dust/io/keycodes.hpp"
#include "glm/ext/vector_float2.hpp"
#include <array>
namespace dust {

class InputManager
{
private:
    std::array<int, DUST_KEY_LAST> m_keys;
    std::array<int, DUST_MOD_COUNT> m_mods;
    std::array<int, DUST_MBUTTON_LAST> m_mbuttons;

    glm::vec2 m_mousePos;

private:
    InputManager();
    ~InputManager();

    inline static Scope<InputManager> s_instance{nullptr};
    
    friend class Window;
    void keyCallback(int key, int scancode, int mods, int action);
    void buttonCallback(int button, int scancode, int mods, int action);
    void mousePosCallback(double x, double y);

    void updateState();

public:
    static InputManager* Get(); 

    void isKey(int key, int keyState) const;
    void isButton(int key, int keyState) const;

    glm::vec2 getMousePos() const;
};

}

#endif //_DUST_IO_INPUTMANAGER_HPP_