#include "dust/core/layer.hpp"
#include "dust/core/log.hpp"
#include "dust/core/types.hpp"
#include "dust/dust.hpp"
#include "dust/io/assetsManager.hpp"
#include "dust/io/inputManager.hpp"
#include "dust/io/keycodes.hpp"
#include "dust/io/loaders.hpp"
#include "dust/render/camera.hpp"
#include "dust/render/framebuffer.hpp"
#include "dust/render/light.hpp"
#include "dust/render/material.hpp"
#include "dust/render/mesh.hpp"
#include "dust/render/model.hpp"
#include "dust/render/shader.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include <cstdlib>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"

#include "dust/render/skybox.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/gtc/type_ptr.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

namespace ImGui {

IMGUI_API bool InputMat4(const char* label, glm::mat4 value, const char* format = "%.3f", ImGuiInputTextFlags flags = ImGuiInputTextFlags_None)
{
    ImGui::SeparatorText(label);
    bool row1 = ImGui::InputFloat4("", glm::value_ptr(value[0]), format, flags);
    bool row2 = ImGui::InputFloat4("", glm::value_ptr(value[1]), format, flags);
    bool row3 = ImGui::InputFloat4("", glm::value_ptr(value[2]), format, flags);
    bool row4 = ImGui::InputFloat4("", glm::value_ptr(value[3]), format, flags);
    return row1 || row2 || row3 || row4;
}

};

const std::vector<glm::vec3> cubesPositions{
    { 0.f, 5.f,   0.f},
    {-3.f, 3.f, -10.f},
    { 2.f, 6.f,  -1.f}
};

using namespace dust;

constexpr f32 CAMERA_ROTATE_SPEED = .5f;

class SimpleApp
: public Application
{
private:
    render::ShaderPtr m_sceneShader;
    render::Framebuffer *m_renderTarget;

    // Shadows
    render::ShaderPtr m_shadowShader;
    render::Framebuffer *m_shadowMap;
    render::DirectionnalLight m_sun;

    render::ModelPtr m_plane;
    std::vector<render::ModelPtr> m_cubes;
    render::Camera3DPtr m_camera;
    render::SkyboxPtr m_skybox;
    glm::vec4 m_ambientColor;

    f32 m_cameraY;
    bool m_wireframe;
    ImVec2 m_previousSize;
public:
    SimpleApp()
    : Application("Cubes", 1920u, 1080u),
    m_wireframe(false),
    m_camera(createRef<render::Camera3D>(getWindow()->getWidth(), getWindow()->getHeight(), 90, 2000)),
    m_sun(glm::normalize(glm::vec3(-2.0f, 4.0f, -1.0f)), {25/255.f, 108/255.f, 225/255.f}),
    m_ambientColor(53/255.f, 35/255.f, 100/255.f, 1.f),
    m_cameraY(10.f)
    { 
        m_camera->makeActive();
        m_sun.updateRenderPos();
        getWindow()->setVSync(false);

        const auto sceneShader = render::Shader::loadFromFile("assets/shader.vert", "assets/shader.frag");
        if(!sceneShader.has_value()) exit(EXIT_FAILURE);
        m_sceneShader = sceneShader.value();

        const auto shadowShader = render::Shader::loadFromFile("assets/depth.vert", "assets/depth.frag");
        if(!shadowShader.has_value()) exit(EXIT_FAILURE);
        m_shadowShader = shadowShader.value();

        m_skybox = render::SkyboxPtr(new render::Skybox({
            "assets/cubemap/right.png", "assets/cubemap/left.png",
            "assets/cubemap/top.png", "assets/cubemap/bottom.png",
            "assets/cubemap/front.png", "assets/cubemap/back.png",
        }));

        m_renderTarget = new render::Framebuffer({
            {
                {
                    render::Framebuffer::AttachmentType::COLOR_RGBA,
                    true
                },
                {
                    render::Framebuffer::AttachmentType::DEPTH_STENCIL,
                    false
                }
            },
            getWindow()->getWidth(),
            getWindow()->getHeight()
        });
        m_previousSize = {(f32)getWindow()->getWidth(), (f32)getWindow()->getHeight()};

        m_shadowMap = new render::Framebuffer({
            {
                {
                    render::Framebuffer::AttachmentType::DEPTH, true
                }
            },
            1024,
            1024
        });
        m_shadowMap->bindAttachment(10, render::Framebuffer::AttachmentType::DEPTH);
        updateUniforms();

        // spawn cubes
        for(const auto& pos : cubesPositions)
        {
            auto cube = render::Mesh::createCube();
            cube->setMaterial(0, createRef<render::ColorMaterial>(glm::vec3{1.f, 1.f, 1.f}));

            auto cubeModel = createRef<render::Model>(cube);
            cubeModel->setPosition({pos.x, pos.y, pos.z});
            m_cubes.push_back(cubeModel);
        }
        auto plane = render::Mesh::createPlane({20.f, 20.f});
        plane->setMaterial(0, createRef<render::ColorMaterial>(glm::vec3{1.f, 1.f, 1.f}));
        m_plane = createRef<render::Model>(plane);
        render::ColorMaterial::SetupMaterialShader(m_sceneShader.get());
        
        // sky color until skybox is created
        getRenderer()->setClearColor(0/255.f, 179/255.f, 255/255.f);
    }

    ~SimpleApp() 
    {
        m_sceneShader.reset();
        m_shadowShader.reset();

        delete m_renderTarget;
        delete m_shadowMap;
        m_skybox.reset();

        for(auto& cube : m_cubes)
        {
            cube.reset();
        }
        m_cubes.clear();
        m_plane.reset();

        m_camera.reset();
    }

    void update() override
    {
        // rotate camera around
        const f32 angle = getTime().time * CAMERA_ROTATE_SPEED;
        m_camera->lookAt(glm::vec3{
            cos(angle) * 10.f, m_cameraY, sin(angle) * 10.f
        }, glm::vec3(0.f));
        m_camera->bind(m_sceneShader.get());
        m_sceneShader->setUniform("uLightViewProj", m_sun.getViewProjMat(true));
    }

    void drawCameraFrustumImmediate(render::CameraPtr camera)
    {
        const auto frustrum = camera->getFrustrum();
        // create mesh
        std::vector<glm::vec3> tris{
            // far plane
            frustrum.farBottomLeft, frustrum.farBottomRight, frustrum.farTopRight,
            frustrum.farTopRight,   frustrum.farTopLeft, frustrum.farBottomLeft,

            // near plane
            frustrum.nearBottomLeft, frustrum.nearBottomRight, frustrum.nearTopRight,
            frustrum.nearTopRight,   frustrum.nearTopLeft, frustrum.nearBottomLeft,

            // bottom plane
            frustrum.nearBottomLeft, frustrum.nearBottomRight, frustrum.farBottomRight,
            frustrum.farBottomRight, frustrum.farBottomLeft, frustrum.nearBottomLeft,

            // top plane
            frustrum.nearTopLeft, frustrum.nearTopRight, frustrum.farTopRight,
            frustrum.farTopRight, frustrum.farTopLeft, frustrum.nearTopLeft,

            // left plane
            frustrum.nearBottomLeft, frustrum.farBottomLeft, frustrum.farTopLeft,
            frustrum.farTopLeft, frustrum.nearTopLeft, frustrum.nearBottomLeft,

            // right plane
            frustrum.nearBottomRight, frustrum.farBottomRight, frustrum.farTopRight,
            frustrum.farTopRight, frustrum.nearTopRight, frustrum.nearBottomRight,
        };
        render::Mesh mesh (tris.data(), sizeof(glm::vec3), tris.size(), { render::Attribute::Pos3D });
        tris.clear();

        getRenderer()->setCulling(false);
        mesh.draw(m_sceneShader.get());
        getRenderer()->setCulling(true);
    }

    void renderScene(ImVec2 size)
    {
        // shadow map
        m_shadowMap->bind();
        {
            getRenderer()->resize(m_shadowMap->getWidth(), m_shadowMap->getHeight());
            getRenderer()->clear(); // only depth

            m_shadowShader->setUniform("uViewProj", m_sun.getViewProjMat());

            for(const auto& cube : m_cubes)
            {
                cube->draw(m_shadowShader.get());
            }
            m_plane->draw(m_shadowShader.get());
        }
        m_shadowMap->unbind();
        m_shadowMap->bindAttachment(10, render::Framebuffer::AttachmentType::DEPTH);
        
        m_renderTarget->bind();
        {
            getRenderer()->resize(size.x, size.y);
            getRenderer()->clear();

            for(const auto& cube : m_cubes)
            {
                cube->draw(m_sceneShader.get());
            }
            m_plane->draw(m_sceneShader.get());

            // draw frustrum
            // render::Camera3DPtr sunCam = createRef<render::Camera3D>(1024,1024,90);
            // sunCam->setProj(m_sun.getProj());
            // sunCam->setView(m_sun.getView());
            // drawCameraFrustumImmediate(sunCam);

            // skybox
            m_skybox->draw(m_camera.get());
        }
        m_renderTarget->unbind();
    }

    void render() override
    {
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // Main Rendering
        {
            if(ImGui::Begin("Scene")){
                const auto size = ImGui::GetContentRegionAvail();
                
                // Viewport resizing handling
                if((u32)size.x != (u32)m_previousSize.x
                || (u32)size.y != (u32)m_previousSize.y) {
                    m_renderTarget->resize(size.x, size.y);
                    m_previousSize = size;
                    m_camera->resize(size.x, size.y);
                    m_camera->bind(m_sceneShader.get()); // update
                }
                
                renderScene(size);

                // render to screen
                const auto renderTexture = m_renderTarget->getAttachment(render::Framebuffer::AttachmentType::COLOR_RGBA);
                ImGui::Text("Size %dx%d", (u32)size.x, (u32)size.y);
                if(renderTexture.has_value()) {
                    ImGui::Image((void*)(u64)(renderTexture->id), size, ImVec2(0, 1), ImVec2(1, 0));
                } else {
                    ImGui::Text("Missing render target texture.");
                }
            }
            ImGui::End();

            if(ImGui::Begin("Inspector")){
                ImGui::Text("FPS: %d", (u32)(1./getTime().delta));

                if(ImGui::CollapsingHeader("Camera")) 
                {
                    ImGui::InputMat4("Projection", m_camera->getProj());
                    ImGui::InputMat4("View", m_camera->getView());
                    ImGui::InputFloat("Camera Y", &m_cameraY, .1f, 1.f);
                }

                if(ImGui::CollapsingHeader("Lighting")) 
                {
                    ImGui::Text("Shadow Map");
                    auto shadowMap = m_shadowMap->getAttachment(render::Framebuffer::AttachmentType::DEPTH);
                    if(shadowMap.has_value()) {
                        ImGui::Image((void*)shadowMap->id, ImVec2{(f32)m_shadowMap->getWidth(), (f32)m_shadowMap->getHeight()}, ImVec2(0, 1), ImVec2(1, 0));
                    } else {
                        ImGui::TextColored(ImVec4(1.f, 0, 0, 1.f), "Couldn't find shadowMap...");
                    }

                    // Sun
                    ImGui::Text("Sun");
                    auto sunDir = m_sun.getDirection();
                    if(ImGui::InputFloat3("Direction", glm::value_ptr(sunDir))) {
                        m_sun.setDirection(sunDir);
                        updateUniforms();
                    }

                    auto sunColor = m_sun.getColor();
                    if(ImGui::ColorEdit3("Color", glm::value_ptr(sunColor))) {
                        m_sun.setColor(sunColor);
                        updateUniforms();
                    }

                    if(ImGui::ColorEdit4("Ambient", glm::value_ptr(m_ambientColor))) {
                        updateUniforms();
                    }

                    if(ImGui::CollapsingHeader("SunMat")) {
                        ImGui::InputMat4("SunViewProj", m_sun.getViewProjMat(), "%.8f");
                        ImGui::InputMat4("SunProj", m_sun.getProj(), "%.8f");
                        ImGui::InputMat4("SunView", m_sun.getView(), "%.8f");
                    }
                }

                if(ImGui::CollapsingHeader("Shaders")) 
                {
                    if(ImGui::Checkbox("Wireframe", &m_wireframe)) {
                        getRenderer()->setDrawWireframe(m_wireframe);
                    }

                    // reload shaders ?
                    if(ImGui::Button("Reload Shaders")) {
                        m_sceneShader->reload();
                        m_shadowShader->reload();

                        updateUniforms();
                    }
                }
            }
            ImGui::End();
        }

        if(ImGui::Begin("Cubes")) {
            int i = 0;
            for(auto& cube : m_cubes) {
                auto pos = cube->getPosition();
                std::string name = "Cube " + std::to_string(i);
                if(ImGui::InputFloat3(name.c_str(), glm::value_ptr(pos))) {
                    cube->setPosition(pos);
                }
                ++i;
            }
        }
        ImGui::End();

        // ImGui::ShowDemoWindow();
    }

private:
    void updateUniforms() {
        m_sun.bind(m_sceneShader);
        m_camera->bind(m_sceneShader.get());
        m_sceneShader->setUniform("uShadowMap", 10);
        m_sceneShader->setUniform("uAmbient", m_ambientColor);
    }
};


DUST_SIMPLE_ENTRY(SimpleApp)

