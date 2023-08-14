#include "dust/core/log.hpp"
#include "dust/dust.hpp"
#include "dust/io/inputManager.hpp"
#include "dust/io/keycodes.hpp"
#include "dust/render/camera.hpp"
#include "dust/render/shader.hpp"

using namespace dust;

constexpr f32 CAMERA_SPEED = 10.f;
constexpr f32 CAMERA_ROTATE_SPEED = 10.f;

class SimpleApp
: public Application
{
private:
    render::Shader *m_shader;
    render::Shader *m_depthShader;
    render::Shader *m_currentShader;

    render::Model *m_sponza;
    render::Camera3D *m_camera;

public:
    SimpleApp()
    : Application("Sponza"),
    m_shader(nullptr),
    m_sponza(nullptr),
    m_camera(new render::Camera3D(getWindow()->getWidth(), getWindow()->getHeight(), 90, 2000))
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

        // reload shaders ?
        if(InputManager::IsKeyPressed(Key::R)) {
            m_shader->reload();
            m_depthShader->reload();
            m_camera->bind(m_shader);
            m_camera->bind(m_depthShader);
        }
        if(InputManager::IsKeyPressed(Key::T)) { // switch shaders
            if(m_currentShader == m_shader) { m_currentShader = m_depthShader; }
            else { m_currentShader = m_shader; }
        }
    }

    void render() override
    {
        m_camera->resize(getWindow()->getWidth(), getWindow()->getHeight());
        m_sponza->draw(m_currentShader);
    }
};

DUST_SIMPLE_ENTRY(SimpleApp)

