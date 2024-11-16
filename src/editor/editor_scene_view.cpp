//
// Created by lucas on 12/11/2024.
//

#include <utility>

#include <glm/glm.hpp>

#include "dust/editor/editor.hpp"
#include "dust/editor/editor_scene_view.hpp"


using namespace dust;

EditorSceneView::EditorSceneView(render::Framebuffer* scene_result_buffer)
    : EditorTool("Scene"), m_scene_result_buffer(scene_result_buffer),
      m_size({0,0}), m_resized(false) {
    if(m_scene_result_buffer != nullptr) {
        m_size = glm::vec2(m_scene_result_buffer->getWidth(), m_scene_result_buffer->getHeight());
    }
}

bool EditorSceneView::is_panel_tool() const { return true; }


void EditorSceneView::render_ui() {
    m_resized = false;
    {
        const auto size = ImGui::GetContentRegionAvail();
        if (m_size.x != size.x || m_size.y != size.y) {
            m_size.x = size.x;
            m_size.y = size.y;
            m_resized = true;
        }

        if(m_scene_result_buffer != nullptr) {
            const auto renderTexture = m_scene_result_buffer->getAttachment(render::Framebuffer::AttachmentType::COLOR_HDR);
            if (renderTexture.has_value()) {
                ImGui::Image((void *)(u64)(renderTexture.value().id), ImVec2((float)m_scene_result_buffer->getWidth(), (float)m_scene_result_buffer->getHeight()), ImVec2(0, 1), ImVec2(1, 0));
            } else {
                ImGui::TextColored(ImVec4{1.f, 0.f, 0.f, 1.f}, "Missing render target texture.");
            }
        } else {
            ImGui::TextColored(ImVec4{1.f, 0.f, 0.f, 1.f}, "Missing render target.");
        }
    }
}

glm::vec2 EditorSceneView::get_size() const {
    return m_size;
}

bool EditorSceneView::was_resized() const {
    return m_resized;
}

void EditorSceneView::set_scene_framebuffer(render::Framebuffer* scene_result_buffer) {
    m_scene_result_buffer = scene_result_buffer;
    m_size = glm::vec2(m_scene_result_buffer->getWidth(), m_scene_result_buffer->getHeight());
}
