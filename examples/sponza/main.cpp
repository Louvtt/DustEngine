#include "dust/core/application.hpp"
#include "dust/core/layer.hpp"
#include "dust/core/log.hpp"
#include "dust/dust.hpp"
#include "dust/io/assetsManager.hpp"
#include "dust/io/inputManager.hpp"
#include "dust/io/keycodes.hpp"
#include "dust/io/loaders.hpp"
#include "dust/render/camera.hpp"
#include "dust/render/framebuffer.hpp"
#include "dust/render/light.hpp"
#include "dust/render/shader.hpp"


#include "dust/render/skybox.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <cstdlib>

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

using namespace dust;

constexpr f32 CAMERA_SPEED = 100.f;
constexpr f32 CAMERA_ROTATE_SPEED = 50.f;

class SponzaApp
: public Application
{
private:
    render::ShaderPtr m_shader;
    render::ShaderPtr m_depthShader;
    render::ShaderPtr m_currentShader;

    ImVec2 m_previousSize;
    render::Framebuffer *m_renderTarget;

    Result<render::ModelPtr> m_sponza;
    render::Camera3DPtr m_camera;
    render::SkyboxPtr m_skybox;

    render::DirectionnalLight m_sun;

    bool m_wireframe;
    bool m_drawSponza;
public:
    SponzaApp()
    : Application("Sponza Demo", 1920u, 1080u),
    m_shader(nullptr),
    m_sponza(),
    m_camera(createRef<render::Camera3D>(getWindow()->getWidth(), getWindow()->getHeight(), 90, 2000)),
    m_sun(glm::normalize(glm::vec3(-2.0f, 4.0f, -1.0f)), {25/255.f, 108/255.f, 225/255.f}),
    m_drawSponza(true), m_wireframe(false), m_renderTarget(nullptr)
    { 
        getWindow()->setVSync(false);

        const auto shader = render::Shader::loadFromFile("assets/shader.vert", "assets/shader.frag");
        if(!shader.has_value()) {
            DUST_ERROR("Exiting ... PBR Shader missing");
            exit(EXIT_FAILURE);
        }
        m_shader = shader.value();

        const auto depthShader = render::Shader::loadFromFile("assets/shader.vert", "assets/depth.frag");
        if(!depthShader.has_value()) {
            DUST_ERROR("Exiting ... Depth Shader missing");
            exit(EXIT_FAILURE);
        }
        m_depthShader = depthShader.value();
        m_currentShader = m_shader; 
        
        m_sponza = io::LoadModel("assets/sponza_pbr/sponza.obj");

        m_camera->setPosition(glm::vec3(0.f, 20.f, 0.f));

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

        updateUniforms();

        // sky color until skybox is created
        m_previousSize = ImVec2{(f32)getWindow()->getWidth(), (f32)getWindow()->getHeight()};
        // getRenderer()->setClearColor(0/255.f, 179/255.f, 255/255.f);
    }

    ~SponzaApp() 
    {
        m_shader.reset();
        m_depthShader.reset();

        delete m_renderTarget;
        m_skybox.reset();

        m_sponza.reset();

        m_camera.reset();
    }

    void update() override
    {
        dust::Application::update();
        f32 delta = (f32)getTime().delta;
        if(InputManager::IsKeyDown(Key::A)) {
            m_camera->rotate({-CAMERA_ROTATE_SPEED * delta, .0f, .0f});
            m_camera->bind(m_currentShader.get());
        }
        if(InputManager::IsKeyDown(Key::D)) {
            m_camera->rotate({CAMERA_ROTATE_SPEED * delta, .0f, .0f});
            m_camera->bind(m_currentShader.get());
        }

        if(InputManager::IsKeyDown(Key::W)) {
            m_camera->move(m_camera->forward() * -CAMERA_SPEED * delta);
            m_camera->bind(m_currentShader.get());
        }
        if(InputManager::IsKeyDown(Key::S)) {
            m_camera->move(m_camera->forward() * CAMERA_SPEED * delta);
            m_camera->bind(m_currentShader.get());
        }
        if(InputManager::IsKeyDown(Key::Space)) {
            m_camera->move(glm::vec3(0, CAMERA_SPEED * delta, 0));
            m_camera->bind(m_currentShader.get());
        }
        if(InputManager::IsKeyDown(Key::LeftShift)) {
            m_camera->move(glm::vec3(0, -CAMERA_SPEED * delta, 0));
            m_camera->bind(m_currentShader.get());
        }
    }

    void render() override
    {
        dust::Application::render();
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // Main Rendering
        {
            if(ImGui::Begin("Scene")){
                const auto size = ImGui::GetContentRegionAvail();

                if(m_previousSize.x != size.x
                || m_previousSize.y != size.y) {
                    m_previousSize = size;
                    getRenderer()->resize(size.x, size.y);
                    m_renderTarget->resize(size.x, size.y);
                    m_camera->resize(size.x, size.y);
                    m_camera->bind(m_currentShader.get()); // update
                }
                
                m_renderTarget->bind();
                {
                    getRenderer()->clear();
                    // sponza
                    if(m_drawSponza && m_sponza.has_value()) {
                        m_sponza.value()->draw(m_currentShader);
                    }

                    // skybox
                    m_skybox->draw(m_camera.get());
                }
                m_renderTarget->unbind();

                // render to screen
                const auto renderTexture = m_renderTarget->getAttachment(render::Framebuffer::AttachmentType::COLOR_RGBA);
                if(renderTexture.has_value()) {
                    ImGui::Image((void*)(u64)(renderTexture->id), size, ImVec2(0, 1), ImVec2(1, 0));
                } else {
                    ImGui::TextColored(ImVec4{1.f, 0.f, 0.f, 1.f}, "Missing render target texture.");
                }
            }
            ImGui::End();

            if(ImGui::Begin("Inspector")){
                ImGui::Text("FPS: %d", (u32)(1./getTime().delta));

                ImGui::SeparatorText("Camera"); 
                {
                    ImGui::InputMat4("Projection", m_camera->getProj());
                    ImGui::InputMat4("View", m_camera->getView());
                }

                ImGui::SeparatorText("Lighting"); 
                {
                    glm::vec3 sunDir = m_sun.getDirection();
                    if(ImGui::InputFloat3("Sun Direction", glm::value_ptr(sunDir), "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
                        if(sunDir.length() != 0) {
                            m_sun.setDirection(sunDir);
                            updateUniforms();
                        }
                    }
                    glm::vec3 sunColor = m_sun.getColor();
                    if(ImGui::ColorEdit3("Sun color", glm::value_ptr(sunColor))) {
                        m_sun.setColor(sunColor);
                        updateUniforms();
                    }
                }

                ImGui::SeparatorText("Shaders");
                {
                    if(ImGui::Checkbox("Wireframe", &m_wireframe)) {
                        getRenderer()->setDrawWireframe(m_wireframe);
                    }
                    ImGui::Checkbox("Draw Sponza", &m_drawSponza);

                    if(ImGui::Button("Show Depth")) { 
                        m_currentShader = m_depthShader; 
                    }
                    if(ImGui::Button("Show Render")) { 
                        m_currentShader = m_shader; 
                    }

                    // reload shaders ?
                    if(ImGui::Button("Reload Shaders")) {
                        m_shader->reload();
                        m_depthShader->reload();

                        updateUniforms();
                    }
                }
            }
            ImGui::End();
        }

        // ImGui::ShowDemoWindow();
    }

private:
    void updateUniforms() {
        m_camera->bind(m_currentShader.get());
        m_sun.bind(m_currentShader);
        m_currentShader->setUniform("uLightCount", 1);
    }
};

DUST_SIMPLE_ENTRY(SponzaApp)

