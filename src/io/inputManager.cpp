#include "dust/io/inputManager.hpp"
#include "GLFW/glfw3.h"
#include "dust/core/log.hpp"
#include "dust/core/types.hpp"
#include "dust/io/keycodes.hpp"
#include "imgui_impl_glfw.h"

dust::InputManager::InputManager(const dust::Window& window)
: m_keys(),
m_mbuttons(),
m_mousePos()
{ 
    auto nativeWindow = window.getNativeWindow();
    s_instance = dust::Scope<dust::InputManager>(this);
    glfwSetKeyCallback(nativeWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods){
        InputManager::Get()->keyCallback(key, scancode, mods, action);
        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    });
    glfwSetMouseButtonCallback(nativeWindow, [](GLFWwindow* window, int button, int action, int mods){
        InputManager::Get()->buttonCallback(button, mods, action);
        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    });
    glfwSetCursorPosCallback(nativeWindow, [](GLFWwindow* window, double x, double y){
        InputManager::Get()->mousePosCallback(x, y);
        ImGui_ImplGlfw_CursorPosCallback(window, x, y);
    });
    // glfwSetScrollCallback(nativeWindow, [](GLFWwindow* window, double xoffset, double yoffset){ });
    DUST_INFO("[InputManager] Events bound.");
}
dust::InputManager::~InputManager()
{
    auto _ = s_instance.release();
}

void dust::InputManager::keyCallback(int key, int scancode, int _mods, int action)
{
    // Unknown key
    if(key < (int)dust::Key::Space || key > (int)dust::Key::Last) return;
    dust::Key dKey = (dust::Key)(key);
    m_keys.at((std::size_t)dKey) = (action == GLFW_PRESS || action == GLFW_REPEAT) ? 
          dust::KeyState::Press 
        : dust::KeyState::Release;
}
void dust::InputManager::buttonCallback(int button, int _mods, int action)
{
    // Unknown button
    if(button > (int)dust::MButton::Last) return;
    dust::MButton dButton = (dust::MButton)(button);
    m_mbuttons.at((std::size_t)dButton) = (action == GLFW_PRESS || action == GLFW_REPEAT) ? 
          dust::KeyState::Press 
        : dust::KeyState::Release;
}
void dust::InputManager::mousePosCallback(double x, double y)
{
    m_mousePos.x = (float)x;
    m_mousePos.y = (float)y;
}

void dust::InputManager::updateState()
{
    for(auto& keyState : m_keys) 
    {
        switch(keyState) {
            case dust::KeyState::Down: break;
            case dust::KeyState::Up: break;
            case dust::KeyState::Release: 
                keyState = dust::KeyState::Up;
                break;
            case dust::KeyState::Press:
                keyState = dust::KeyState::Down;
                break;
        }
    }

    for(auto& buttonState : m_keys) 
    {
        switch(buttonState) {
            case dust::ButtonState::Down: break;
            case dust::ButtonState::Up: break;
            case dust::ButtonState::Release: 
                buttonState = dust::ButtonState::Up;
                break;
            case dust::ButtonState::Press:
                buttonState = dust::ButtonState::Down;
                break;
        }
    }
}

dust::InputManager* dust::InputManager::Get()
{
    return s_instance.get();
} 

bool dust::InputManager::isKey(dust::Key key, dust::KeyState keyState) const
{
    return m_keys.at((std::size_t)key) == keyState;
}
bool dust::InputManager::isButton(dust::MButton button, dust::ButtonState buttonState) const
{
    return m_mbuttons.at((std::size_t)button) == buttonState;
}

glm::vec2 dust::InputManager::getMousePos() const
{
    return m_mousePos;
}

bool dust::InputManager::IsKeyPressed(dust::Key key)  
{ return s_instance->isKey(key, dust::KeyState::Press); }
bool dust::InputManager::IsKeyDown(dust::Key key)     
{ return s_instance->isKey(key, dust::KeyState::Down); }
bool dust::InputManager::IsKeyReleased(dust::Key key) 
{ return s_instance->isKey(key, dust::KeyState::Release); }
bool dust::InputManager::IsKeyUp(dust::Key key)       
{ return s_instance->isKey(key, dust::KeyState::Up); }