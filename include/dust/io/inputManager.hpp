#ifndef _DUST_IO_INPUTMANAGER_HPP_
#define _DUST_IO_INPUTMANAGER_HPP_

#include "dust/core/types.hpp"
#include "dust/io/keycodes.hpp"
#include "glm/ext/vector_float2.hpp"
#include <array>
#include <cstddef>

#include "dust/core/window.hpp"

namespace dust {
class Application;
class InputManager
{
private:
    std::array<dust::KeyState, (std::size_t)dust::Key::Last> m_keys;
    std::array<dust::ButtonState, (std::size_t)dust::MButton::Last> m_mbuttons;

    glm::vec2 m_mousePos;

    inline static Scope<InputManager> s_instance{nullptr};
    
    void keyCallback(int key, int scancode, int mods, int action);
    void buttonCallback(int button, int mods, int action);
    void mousePosCallback(double x, double y);

    void updateState();

public:
    InputManager(const Window& window);
    ~InputManager();

    static InputManager* Get(); 

    bool isKey(Key key, KeyState keyState) const;
    bool isButton(MButton key, ButtonState keyState) const;

    glm::vec2 getMousePos() const;
};

}

#endif //_DUST_IO_INPUTMANAGER_HPP_