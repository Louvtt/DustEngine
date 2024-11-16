//
// Created by lucas on 12/11/2024.
//

#ifndef _DUST_EDITOR_MODEL_TOOL_HPP_
#define _DUST_EDITOR_MODEL_TOOL_HPP_

#include "dust/editor/editor.hpp"
#include "dust/render/model.hpp"

namespace dust {

class ModelTool : public EditorTool {
private:
    render::Model* m_inspected_model;

public:
    ModelTool();
    ~ModelTool() override = default;

    void render_ui() override;
    void set_inspected_model(render::Model *model);
};

}

#endif //_DUST_EDITOR_MODEL_TOOL_HPP_
