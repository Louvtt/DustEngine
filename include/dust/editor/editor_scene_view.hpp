//
// Created by lucas on 12/11/2024.
//

#ifndef DUSTENGINE_INCLUDE_DUST_EDITOR_EDITOR_SCENE_VIEW_HPP
#define DUSTENGINE_INCLUDE_DUST_EDITOR_EDITOR_SCENE_VIEW_HPP

#include "editor.hpp"

#include "dust/render/framebuffer.hpp"

namespace dust {

class EditorSceneView : public EditorTool {
private:
    render::Framebuffer* m_scene_result_buffer{nullptr};
    glm::vec2 m_size;
    bool m_resized;

public:
    explicit EditorSceneView(render::Framebuffer* scene_result_buffer);
    ~EditorSceneView() override = default;

    void render_ui() override;
    void set_scene_framebuffer(render::Framebuffer* scene_result_buffer);

    bool was_resized() const;
    glm::vec2 get_size() const;
    bool is_panel_tool() const override;
};

};

#endif // DUSTENGINE_INCLUDE_DUST_EDITOR_EDITOR_SCENE_VIEW_HPP
