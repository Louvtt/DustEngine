//
// Created by lucas on 12/11/2024.
//

#include "dust/editor/model_tool.hpp"
#include "dust/editor/imgui_extensions.hpp"

#include <format>

using namespace dust;

ModelTool::ModelTool()
    : EditorTool("Model Inspector"), m_inspected_model(nullptr) {}

void ModelTool::render_ui() {
    if(m_inspected_model == nullptr) {
        ImGui::TextColored(ImVec4{1.f, 0.f, 0.f, 1.f}, "No model to inspect.");
        return;
    }

    u32 i = 0;
    for (auto &mesh : m_inspected_model->getMeshes()) {
        if (!mesh.get()) {
            ++i;
            continue;
        }
        const auto meshName = std::format("Mesh {}", i);
        if (ImGui::TreeNode(meshName.c_str())) {
            ImGui::TextWrapped("Name: %s", mesh->getName().c_str());
            bool hidden = mesh->isHidden();
            if (ImGui::Checkbox("Hidden", &hidden)) mesh->setHidden(hidden);
            if (ImGui::TreeNode("Materials")) {
                u32 j = 0;
                for (auto &mat : mesh->getMaterials()) {
                    if (!mat) {
                        j++;
                        continue;
                    }
                    // Show material
                    const auto matName = std::format("PBR Material {} - {}", j, mat->getName());
                    if (ImGui::TreeNode(matName.c_str())) {
                        auto *m = (render::PBRMaterial *)mat.get();
                        ImGui::ColorEdit3("Albedo", glm::value_ptr(m->albedo));
                        ImGui::SliderFloat("Roughness", &(m->roughness), 0.0f, 1.0f);
                        ImGui::SliderFloat("Metallic", &(m->metallic), 0.0f, 1.0f);
                        ImGui::SliderFloat("AO", &(m->ao), 0.0f, 1.0f);
                        if (ImGui::TreeNode("Textures")) {
                            ImGui::TextureLabelled("Albedo", m->albedoTexture.get());
                            ImGui::TextureLabelled("Normal", m->normalTexture.get());
                            ImGui::TextureLabelled("Metalness", m->metallicTexture.get());
                            ImGui::TextureLabelled("Roughness", m->roughnessTexture.get());
                            ImGui::TextureLabelled("AO", m->aoTexture.get());
                            ImGui::TreePop();
                        }
                        ImGui::TreePop();
                    }
                    j++;
                }
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
        i++;
    }
}

void ModelTool::set_inspected_model(render::Model *model) {

}
