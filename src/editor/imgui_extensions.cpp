//
// Created by lucas on 12/11/2024.
//

#include "dust/editor/imgui_extensions.hpp"

IMGUI_API bool ImGui::InputMat4(const char *label, glm::mat4 value, const char *format, ImGuiInputTextFlags flags) {
    ImGui::SeparatorText(label);
    bool row1 = ImGui::InputFloat4("", glm::value_ptr(value[0]), format, flags);
    bool row2 = ImGui::InputFloat4("", glm::value_ptr(value[1]), format, flags);
    bool row3 = ImGui::InputFloat4("", glm::value_ptr(value[2]), format, flags);
    bool row4 = ImGui::InputFloat4("", glm::value_ptr(value[3]), format, flags);
    return row1 || row2 || row3 || row4;
}

IMGUI_API void ImGui::TextureLabelled(const char *label, dust::render::Texture *texture) {
    ImGui::BeginGroup();
    ImGui::Text("Texture: %s", label);
    ImGui::Image((void *)texture->getRenderID(),
                 ImVec2{100, 100.f * ((float)texture->getHeight() / texture->getWidth())});
    ImGui::EndGroup();
}