//
// Created by louvtt on 7/23/24.
//

#include "dust/scripting/script.hpp"

#include "dust/core/log.hpp"
#include "dust/core/application.hpp"

#include <tuple>
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <filesystem>
namespace fs = std::filesystem;

dust::Script::Script(const std::string &filename) : io::ResourceFile(filename) {}

void dust::Script::set_value(const std::string &name, int value) { }

void dust::Script::set_value(const std::string &name, float value) { }

void dust::Script::set_value(const std::string &name, const std::string &value) { }

void dust::Script::reload(bool first_load) {
    DUST_DEBUG("Script reload: {}", filepath.c_str());
    auto state = Application::Get()->getScriptingManager()->get_lua_state();
    if(fs::exists(filepath)) {
        auto result = state->safe_script_file(filepath);
        if(result.valid() && static_cast<int>(result.get_type()) == LUA_TTABLE) {
            script_table = result.get<sol::table>();
            DUST_DEBUG("Script: Loaded script file {}", filepath.c_str());
        } else {
            DUST_ERROR("Script: Error while loading file {}", filepath.c_str());
        }
    } else {
        DUST_ERROR("Script: Failed to load script file {}", filepath.c_str());
    }

}