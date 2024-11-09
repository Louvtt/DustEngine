//
// Created by louvtt on 9/23/24.
//

#ifndef _DUST_SCRIPTING_SCRIPTINGMANAGER_HPP_
#define _DUST_SCRIPTING_SCRIPTINGMANAGER_HPP_

#include "dust/core/types.hpp"
#include <sol/sol.hpp>

namespace dust {
    class ScriptingManager {
    private:
        sol::state lua_state;

    public:
        using Ptr = std::shared_ptr<ScriptingManager>;

        ScriptingManager();

        ~ScriptingManager();

        void register_default_systems();

        [[nodiscard]] sol::state* get_lua_state();
    };
}

#endif //_DUST_SCRIPTING_SCRIPTINGMANAGER_HPP_
