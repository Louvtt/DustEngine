//
// Created by louvtt on 7/23/24.
//

#ifndef _DUST_SCRIPTING_SCRIPT_HPP_
#define _DUST_SCRIPTING_SCRIPT_HPP_

#include "dust/core/types.hpp"
#include "dust/core/log.hpp"
#include "dust/core/application.hpp"

#include <variant>
#include <sol/sol.hpp>

#include "dust/io/resourceFile.hpp"

namespace dust {

class Script : public io::ResourceFile {
    private:
        sol::table script_table;

    public:
        explicit Script(const std::string& filename);

        ~Script() override = default;

        void set_value(const std::string& name, int value);

        void set_value(const std::string& name, float value);

        void set_value(const std::string& name, const std::string& value);

        template<typename ReturnType, typename ...Args>
        auto call_function(const std::string& function_name, Args&& ...args) -> ReturnType
        {
            if (!script_table.valid()) return;

            const auto fn = script_table.get<sol::optional<sol::function >> (function_name);
            if (fn.has_value()) {
                fn.value()(std::forward<Args>(args)...);
            }
        }

        void reload(bool first_load) override;
    };
}


#endif //_DUST_SCRIPTING_SCRIPT_HPP_
