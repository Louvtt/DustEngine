#include "dust/core/layer.hpp"
#include "dust/core/log.hpp"
#include "dust/dust.hpp"
#include "dust/io/inputManager.hpp"
#include "dust/io/keycodes.hpp"
#include "dust/render/camera.hpp"
#include "dust/render/shader.hpp"

#include "glm/ext/quaternion_geometric.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

using namespace dust;

constexpr f32 CAMERA_SPEED = 50.f;
constexpr f32 CAMERA_ROTATE_SPEED = 50.f;

class SimpleApp
: public Application
{
private:
    render::Shader *m_shader;
    render::Shader *m_depthShader;
    render::Shader *m_currentShader;

    render::Model *m_sponza;
    render::Camera3D *m_camera;

    glm::vec3 m_sunDirection;
    glm::vec4 m_sunColor;
    glm::vec4 m_ambientColor;

public:
    SimpleApp()
    : Application("Sponza"),
    m_shader(nullptr),
    m_sponza(nullptr),
    m_camera(new render::Camera3D(getWindow()->getWidth(), getWindow()->getHeight(), 90, 2000)),
    m_sunDirection(0.f, 1.f, 0.f),
    m_sunColor(1.f)
    { 
        m_shader = render::Shader::loadFromFile("assets/shader.vert", "assets/shader.frag");
        if(!(bool)m_shader) {
            DUST_ERROR("Exiting ... Shader missing");
            exit(-1);
        }
        m_depthShader = render::Shader::loadFromFile("assets/shader.vert", "assets/depth.frag");
        if(!(bool)m_depthShader) {
            DUST_ERROR("Exiting ... Shader missing");
            exit(-1);
        }
        m_currentShader = m_depthShader;
        
        m_sponza = render::Model::loadFromFile("assets/sponza/sponza.obj");
        if(!m_sponza) {
            DUST_ERROR("Exiting ... Sponza missing");
            exit(-1);
        }

        m_camera->setPosition(glm::vec3(0.f, 10.f, 0.f));
        m_camera->bind(m_currentShader);
    }

    void update() override
    {
        f32 delta = (f32)getTime().delta;
        if(InputManager::IsKeyDown(Key::A)) {
            m_camera->rotate({-CAMERA_ROTATE_SPEED * delta, .0f, .0f});
            m_camera->bind(m_currentShader);
        }
        if(InputManager::IsKeyDown(Key::D)) {
            m_camera->rotate({CAMERA_ROTATE_SPEED * delta, .0f, .0f});
            m_camera->bind(m_currentShader);
        }

        if(InputManager::IsKeyDown(Key::W)) {
            m_camera->move(m_camera->forward() * CAMERA_SPEED * delta);
            m_camera->bind(m_currentShader);
        }
        if(InputManager::IsKeyDown(Key::S)) {
            m_camera->move(m_camera->forward() * -CAMERA_SPEED * delta);
            m_camera->bind(m_currentShader);
        }
        if(InputManager::IsKeyDown(Key::Space)) {
            m_camera->move(glm::vec3(0, CAMERA_SPEED * delta, 0));
            m_camera->bind(m_currentShader);
        }
        if(InputManager::IsKeyDown(Key::LeftShift)) {
            m_camera->move(glm::vec3(0, -CAMERA_SPEED * delta, 0));
            m_camera->bind(m_currentShader);
        }
    }

    void render() override
    {
        if(ImGui::Begin("Debug")){
            if(ImGui::BeginChild("Camera", ImVec2{0, 50})) {
                // some values ?
                ImGui::Text("I will add the camera transform later.");
            }
            ImGui::EndChild();
            // ImGui::Separator();
            if(ImGui::BeginChild("Ligthing", ImVec2{0, 100})) {
                if(ImGui::InputFloat3("Sun Direction", glm::value_ptr(m_sunDirection), "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
                    if(m_sunDirection.length() == 0) {
                        m_sunDirection = glm::vec3(0, 1, 0); // reset
                    }
                    m_currentShader->setUniform("uSunDirection", glm::normalize(m_sunDirection));
                }
                if(ImGui::ColorEdit4("Sun color", glm::value_ptr(m_sunColor), ImGuiColorEditFlags_DisplayRGB)) {
                    m_currentShader->setUniform("uSunColor", m_sunColor);
                }
                if(ImGui::ColorEdit4("Ambient color", glm::value_ptr(m_ambientColor), ImGuiColorEditFlags_DisplayRGB)) {
                    m_currentShader->setUniform("uAmbient", m_ambientColor);
                }
            }
            ImGui::EndChild();
            // ImGui::Separator();
            if(ImGui::BeginChild("Shaders", ImVec2{0, 100})) {
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

                    m_shader->setUniform("uSunDirection", glm::normalize(m_sunDirection));
                    m_shader->setUniform("uSunColor", m_sunColor);
                    m_shader->setUniform("uAmbient", m_ambientColor);
                }

            }
            ImGui::EndChild();
        }
        ImGui::End();

        m_camera->resize(getWindow()->getWidth(), getWindow()->getHeight());
        m_camera->bind(m_currentShader); // update
        m_sponza->draw(m_currentShader);
    }
};

DUST_SIMPLE_ENTRY(SimpleApp)

