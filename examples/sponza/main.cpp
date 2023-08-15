#include "dust/core/layer.hpp"
#include "dust/core/log.hpp"
#include "dust/dust.hpp"
#include "dust/io/inputManager.hpp"
#include "dust/io/keycodes.hpp"
#include "dust/io/loaders.hpp"
#include "dust/render/camera.hpp"
#include "dust/render/shader.hpp"

#include "dust/render/skybox.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

using namespace dust;

constexpr f32 CAMERA_SPEED = 100.f;
constexpr f32 CAMERA_ROTATE_SPEED = 50.f;

class SimpleApp
: public Application
{
private:
    Ref<render::Shader> m_shader;
    Ref<render::Shader> m_depthShader;
    render::Shader *m_currentShader;

    Ref<render::Model> m_sponza;
    Ref<render::Camera3D> m_camera;
    render::Skybox *m_skybox;

    glm::vec3 m_sunDirection;
    glm::vec4 m_sunColor;
    glm::vec4 m_ambientColor;

    bool m_drawSponza;
public:
    SimpleApp()
    : Application("Sponza"),
    m_shader(nullptr),
    m_sponza(nullptr),
    m_camera(createRef<render::Camera3D>(getWindow()->getWidth(), getWindow()->getHeight(), 90, 2000)),
    m_sunDirection(-.5f, .5f, 0.f),
    m_sunColor(234/255.f, 198/255.f, 147/255.f, 1.f),
    m_ambientColor(4/255.f, 0/255.f, 14/255.f, 1.f),
    m_drawSponza(true)
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
        m_currentShader = m_shader.get();
        
        m_sponza = io::ModelLoader::Read("assets/sponza/sponza.obj");
        if(!m_sponza) {
            DUST_ERROR("Exiting ... Sponza missing");
            exit(-1);
        }

        m_camera->setPosition(glm::vec3(0.f, 20.f, 0.f));

        m_skybox = new render::Skybox({
            "assets/cubemap/right.png", "assets/cubemap/left.png",
            "assets/cubemap/bottom.png", "assets/cubemap/top.png",
            "assets/cubemap/front.png", "assets/cubemap/back.png",
        });

        m_camera->bind(m_currentShader);
        updateUniforms();

        // sky color until skybox is created
        getRenderer()->setClearColor(0/255.f, 179/255.f, 255/255.f);
    }

    ~SimpleApp() 
    {
        m_shader.reset();
        m_depthShader.reset();

        m_sponza.reset();

        m_camera.reset();
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
            ImGui::Text("FPS: %d", (u32)(1./getTime().delta));

            ImGui::SeparatorText("Camera");
            if(ImGui::BeginChild("Camera", ImVec2{0, 50})) {
                // some values ?
                ImGui::Text("I will add the camera transform later.");
            }
            ImGui::EndChild();
            ImGui::SeparatorText("Lighting");
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
            ImGui::SeparatorText("Shaders");
            if(ImGui::BeginChild("Shaders", ImVec2{0, 100})) {
                bool wireframeEnabled;
                if(ImGui::Checkbox("Wireframe", &wireframeEnabled)) {
                    getRenderer()->setDrawWireframe(wireframeEnabled);
                }
                ImGui::Checkbox("Draw Sponza", &m_drawSponza);

                if(ImGui::Button("Show Depth")) { 
                    m_currentShader = m_depthShader.get(); 
                }
                if(ImGui::Button("Show Render")) { 
                    m_currentShader = m_shader.get(); 
                }

                // reload shaders ?
                if(ImGui::Button("Reload Shaders")) {
                    m_shader->reload();
                    m_depthShader->reload();

                    updateUniforms();
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();

        m_camera->resize(getWindow()->getWidth(), getWindow()->getHeight());
        m_camera->bind(m_currentShader); // update
        if(m_drawSponza) {
            m_sponza->draw(m_currentShader);
        }

        m_skybox->draw(m_camera.get());
    }

private:
    void updateUniforms() {
        m_shader->setUniform("uSunDirection", glm::normalize(m_sunDirection));
        m_shader->setUniform("uSunColor", m_sunColor);
        m_shader->setUniform("uAmbient", m_ambientColor);
    }
};

DUST_SIMPLE_ENTRY(SimpleApp)

