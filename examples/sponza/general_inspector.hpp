//
// Created by lucas on 12/11/2024.
//

#ifndef DUSTENGINE_EXAMPLES_SPONZA_GENERAL_INSPECTOR_HPP
#define DUSTENGINE_EXAMPLES_SPONZA_GENERAL_INSPECTOR_HPP

#include "dust/editor/editor.hpp"

class GeneralInspector : public dust::EditorTool {
public:
    GeneralInspector();

    [[nodiscard]] bool is_panel_tool() const override;
    void render_ui() override;
};

#endif // DUSTENGINE_EXAMPLES_SPONZA_GENERAL_INSPECTOR_HPP
