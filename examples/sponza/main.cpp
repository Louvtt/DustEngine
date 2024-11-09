#include "dust/core/application.hpp"
#include "dust/core/layer.hpp"
#include "dust/core/log.hpp"
#include "dust/core/profiling.hpp"
#include "dust/dust.hpp"
#include "dust/io/assetsManager.hpp"
#include "dust/io/inputManager.hpp"
#include "dust/io/keycodes.hpp"
#include "dust/io/loaders.hpp"
#include "dust/render/camera.hpp"
#include "dust/render/framebuffer.hpp"
#include "dust/render/light.hpp"
#include "dust/render/material.hpp"
#include "dust/render/renderPass.hpp"
#include "dust/render/shader.hpp"
#include "dust/render/texture.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"

#include "dust/render/skybox.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cstdlib>
#include <format>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

namespace ImGui {

IMGUI_API bool InputMat4(const char *label, glm::mat4 value, const char *format = "%.3f",
                         ImGuiInputTextFlags flags = ImGuiInputTextFlags_None) {
    ImGui::SeparatorText(label);
    bool row1 = ImGui::InputFloat4("", glm::value_ptr(value[0]), format, flags);
    bool row2 = ImGui::InputFloat4("", glm::value_ptr(value[1]), format, flags);
    bool row3 = ImGui::InputFloat4("", glm::value_ptr(value[2]), format, flags);
    bool row4 = ImGui::InputFloat4("", glm::value_ptr(value[3]), format, flags);
    return row1 || row2 || row3 || row4;
}

IMGUI_API void TextureLabelled(const char *label, render::Texture *texture) {
    ImGui::BeginGroup();
    ImGui::Text("Texture: %s", label);
    ImGui::Image((void *)texture->getRenderID(),
                 ImVec2{100, 100.f * ((float)texture->getHeight() / texture->getWidth())});
    ImGui::EndGroup();
}

}; // namespace ImGui

using namespace dust;

constexpr f32 CAMERA_SPEED        = 100.f;
constexpr f32 CAMERA_ROTATE_SPEED = 50.f;

class SponzaApp : public Application {
private:
    render::ShaderPtr m_shader;
    render::ShaderPtr m_depthShader;
    render::ShaderPtr m_currentShader;

    ImVec2 m_previousSize;

    Result<render::ModelPtr> m_sponza;
    render::Camera3DPtr m_camera;
    render::SkyboxPtr m_skybox;

    render::DirectionnalLight m_sun;

    render::RenderPassPtr m_simplePass;
    render::PostProcessPassPtr m_postprocessPass;

    bool m_wireframe;
    bool m_drawSponza;
    float m_exposure;

public:
    SponzaApp()
        : Application("Sponza Demo", 1920u, 1080u), m_shader(nullptr), m_sponza(),
          m_camera(createRef<render::Camera3D>(getWindow()->getWidth(), getWindow()->getHeight(),
                                               90, 2000)),
          m_sun(glm::normalize(glm::vec3(-2.0f, 4.0f, -1.0f)),
                {25 / 255.f, 108 / 255.f, 225 / 255.f}),
          m_drawSponza(true), m_wireframe(false), m_simplePass(nullptr),
          m_postprocessPass(nullptr),
          m_exposure(1.) {
        getWindow()->setVSync(false);

        const auto shader =
            render::PackedShader::LoadFromFile("assets/pbr.glsl");
        if (!shader.has_value()) {
            DUST_ERROR("Exiting ... PBR Shader missing");
            exit(EXIT_FAILURE);
        }
        m_shader = shader.value();

        const auto depthShader =
            render::Shader::LoadFromFile("assets/shader.vert", "assets/depth.frag");
        if (!depthShader.has_value()) {
            DUST_ERROR("Exiting ... Depth Shader missing");
            exit(EXIT_FAILURE);
        }
        m_depthShader   = depthShader.value();
        m_currentShader = m_shader;

        m_sponza = io::LoadModel("assets/sponza_pbr/sponza.obj");
        render::PBRMaterial::SetupMaterialShader(m_shader.get());

        m_camera->setPosition(glm::vec3(0.f, 20.f, 0.f));

        m_skybox = render::SkyboxPtr(new render::Skybox({
            "assets/cubemap/right.png",
            "assets/cubemap/left.png",
            "assets/cubemap/top.png",
            "assets/cubemap/bottom.png",
            "assets/cubemap/front.png",
            "assets/cubemap/back.png",
        }));

        m_simplePass = createRef<render::RenderPass>(render::RenderPassDesc{
            m_currentShader, createRef<render::Framebuffer>(render::Framebuffer::Desc{
                {{render::Framebuffer::AttachmentType::COLOR_HDR, true},
                {render::Framebuffer::AttachmentType::DEPTH_STENCIL, false}},
                getWindow()->getWidth(),
                getWindow()->getHeight()}
            )
        });

        updateUniforms();

        // sky color until skybox is created
        m_previousSize = ImVec2{(f32)getWindow()->getWidth(), (f32)getWindow()->getHeight()};
        // getRenderer()->setClearColor(0/255.f, 179/255.f, 255/255.f);
    }

    ~SponzaApp() {
        m_shader.reset();
        m_depthShader.reset();

        m_simplePass.reset();
        m_postprocessPass.reset();
        m_skybox.reset();

        m_sponza.reset();

        m_camera.reset();
    }

    void update() override {
        dust::Application::update();
        f32 delta = (f32)getTime().delta;
        if (InputManager::IsKeyDown(Key::A)) {
            m_camera->rotate({-CAMERA_ROTATE_SPEED * delta, .0f, .0f});
            m_camera->bind(m_currentShader.get());
        }
        if (InputManager::IsKeyDown(Key::D)) {
            m_camera->rotate({CAMERA_ROTATE_SPEED * delta, .0f, .0f});
            m_camera->bind(m_currentShader.get());
        }

        if (InputManager::IsKeyDown(Key::W)) {
            m_camera->move(m_camera->forward() * -CAMERA_SPEED * delta);
            m_camera->bind(m_currentShader.get());
        }
        if (InputManager::IsKeyDown(Key::S)) {
            m_camera->move(m_camera->forward() * CAMERA_SPEED * delta);
            m_camera->bind(m_currentShader.get());
        }
        if (InputManager::IsKeyDown(Key::Space)) {
            m_camera->move(glm::vec3(0, CAMERA_SPEED * delta, 0));
            m_camera->bind(m_currentShader.get());
        }
        if (InputManager::IsKeyDown(Key::LeftShift)) {
            m_camera->move(glm::vec3(0, -CAMERA_SPEED * delta, 0));
            m_camera->bind(m_currentShader.get());
        }
    }

    void editorRender() {
#ifndef EXAMPLE_REMOVE_EDITOR
        DUST_PROFILE_SECTION("ImGui Editor");
        if (ImGui::Begin("Inspector")) {
            ImGui::Text("FPS: %d", (u32)(1. / getTime().delta));

            ImGui::SeparatorText("Camera");
            {
                ImGui::InputMat4("Projection", m_camera->getProj());
                ImGui::InputMat4("View", m_camera->getView());
            }

            ImGui::SeparatorText("Lighting");
            {

                ImGui::SliderFloat("Exposure", &m_exposure, .1, 5., "%.2f");

                glm::vec3 sunDir = m_sun.getDirection();
                if (ImGui::InputFloat3("Sun Direction", glm::value_ptr(sunDir), "%.3f",
                                       ImGuiInputTextFlags_EnterReturnsTrue)) {
                    if (sunDir.length() != 0) {
                        m_sun.setDirection(sunDir);
                        updateUniforms();
                    }
                }
                glm::vec3 sunColor = m_sun.getColor();
                if (ImGui::ColorEdit3("Sun color", glm::value_ptr(sunColor))) {
                    m_sun.setColor(sunColor);
                    updateUniforms();
                }
            }

            ImGui::SeparatorText("Shaders");
            {
                if (ImGui::Checkbox("Wireframe", &m_wireframe)) {
                    getRenderer()->setDrawWireframe(m_wireframe);
                }
                ImGui::Checkbox("Draw Sponza", &m_drawSponza);

                if (ImGui::Button("Show Depth")) {
                    m_currentShader = m_depthShader;
                }
                if (ImGui::Button("Show Render")) {
                    m_currentShader = m_shader;
                }

                // reload shaders ?
                if (ImGui::Button("Reload Shaders")) {
                    m_shader->reload(false);
                    m_depthShader->reload(false);
                    render::PBRMaterial::SetupMaterialShader(m_shader.get());
                    updateUniforms();
                }
            }
        }
        ImGui::End();

        if (ImGui::Begin("Model Data")) {
            u32 i = 0;
            for (auto &mesh : m_sponza.value()->getMeshes()) {
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
                            if (!mat.get()) {
                                j++;
                                continue;
                            }
                            // Show material
                            const auto matName =
                                std::format("PBR Material {} - {}", j, mat->getName());
                            if (ImGui::TreeNode(matName.c_str())) {
                                render::PBRMaterial *m = (render::PBRMaterial *)mat.get();
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
        ImGui::End();
#endif
    }

    void render() override {
        dust::Application::render();
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // Main Rendering
        {
            DUST_PROFILE_SECTION("Scene Render");
            if (ImGui::Begin("Scene")) {
                const auto size = ImGui::GetContentRegionAvail();

                if (m_previousSize.x != size.x || m_previousSize.y != size.y) {
                    m_previousSize = size;
                    getRenderer()->resize(size.x, size.y);
                    m_simplePass->getFramebuffer()->resize(size.x, size.y);
                    m_camera->resize(size.x, size.y);
                    m_camera->bind(m_currentShader.get()); // update
                }

                m_simplePass->preRender();
                {
                    DUST_PROFILE_GPU("Sponza render");
                    getRenderer()->clear();
                    // sponza
                    if (m_drawSponza && m_sponza.has_value()) {
                        m_sponza.value()->draw(m_currentShader.get());
                    }

                    // skybox
                    m_skybox->draw(m_camera.get());
                }
                m_simplePass->postRender();

                // render to screen
                const auto renderTexture = m_simplePass->getFramebuffer()->getAttachment(
                    render::Framebuffer::AttachmentType::COLOR_HDR);
                if (renderTexture.has_value()) {
                    ImGui::Image((void *)(u64)(renderTexture->id), size, ImVec2(0, 1),
                                 ImVec2(1, 0));
                } else {
                    ImGui::TextColored(ImVec4{1.f, 0.f, 0.f, 1.f},
                                       "Missing render target texture.");
                }
            }
            ImGui::End();
        }
        editorRender();
    }

private:
    void updateUniforms() {
        m_camera->bind(m_currentShader.get());
        m_sun.bind(m_currentShader);
        m_currentShader->setUniform("uLightCount", 1);
        m_currentShader->setUniform("uExposure", m_exposure);
    }
};

DUST_SIMPLE_ENTRY(SponzaApp)
