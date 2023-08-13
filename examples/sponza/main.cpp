#include "dust/core/log.hpp"
#include "dust/dust.hpp"
#include "dust/io/inputManager.hpp"
#include "dust/io/keycodes.hpp"
#include "dust/render/camera.hpp"

using namespace dust;

class SimpleApp
: public Application
{
private:
    render::Shader *m_shader;
    render::Model *m_sponza;
    render::Camera3D *m_camera;

public:
    SimpleApp()
    : Application("Sponza"),
    m_shader(nullptr),
    m_sponza(nullptr),
    m_camera(new render::Camera3D(getWindow()->getWidth(), getWindow()->getHeight(), 90))
    { 
        m_shader = render::Shader::loadFromFile("assets/shader.vert", "assets/shader.frag");
        if(!(bool)m_shader) {
            DUST_ERROR("Exiting ... Shader missing");
            exit(-1);
        }

        m_sponza = render::Model::loadFromFile("assets/sponza/sponza.obj");
        if(!m_sponza) {
            DUST_ERROR("Exiting ... Sponza missing");
            exit(-1);
        }

        m_camera->bind(m_shader);
    }

    void update() override
    {
        f32 delta = (f32)getTime().delta;
        if(InputManager::IsKeyDown(Key::Q)) {
            m_camera->rotate({-10.f * delta, .0f, .0f});
        }
        if(InputManager::IsKeyDown(Key::D)) {
            m_camera->rotate({10.f * delta, .0f, .0f});
        }

        if(InputManager::IsKeyDown(Key::W)) {
            m_camera->move(m_camera->forward() * 100.f * delta);
        }
        if(InputManager::IsKeyDown(Key::S)) {
            m_camera->move(m_camera->forward() * -100.f * delta);
        }
    }

    void render() override
    {
        m_camera->resize(getWindow()->getWidth(), getWindow()->getHeight());
        m_sponza->draw(m_shader);
    }
};

DUST_SIMPLE_ENTRY(SimpleApp)

