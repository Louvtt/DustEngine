//
// Created by louvtt on 9/23/24.
//

#include "dust/scripting/scriptingManager.hpp"

#include "dust/core/log.hpp"

#include "dust/core/application.hpp"
#include "dust/io/inputManager.hpp"

dust::ScriptingManager::ScriptingManager() {
    lua_state.open_libraries(sol::lib::base);
    DUST_INFO("Loaded Lua library");
}

dust::ScriptingManager::~ScriptingManager() {
}

void dust::ScriptingManager::register_default_systems() {
    // input system
    {
        lua_state.new_usertype<InputManager>("Input",
            "get_instance", &InputManager::Get,
            "is_key_down", &InputManager::IsKeyDown,
            "is_key_released", &InputManager::IsKeyReleased,
            "is_key_up", &InputManager::IsKeyUp,
            "is_key_pressed", &InputManager::IsKeyPressed
        );
    }
}

sol::state* dust::ScriptingManager::get_lua_state() {
    return &lua_state;
}