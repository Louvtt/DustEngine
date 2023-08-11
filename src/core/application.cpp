#include "dust/core/application.hpp"
#include "GLFW/glfw3.h"
#include "dust/core/types.hpp"
#include "dust/core/log.hpp"
#include "dust/render/renderer.hpp"

#include <memory>

dust::Application::Application(const std::string& name, u32 width, u32 height)
: m_name(name),
m_time()
{
    if(s_instance) {
        DUST_ERROR("Cannot create another application instance");
        return;
    }

    m_window = dust::createScope<dust::Window>(name, width, height);
    m_renderer = dust::createScope<dust::Renderer>(*m_window);
    
    s_instance = this;
}

dust::Application::~Application()
{
    m_renderer.reset();
    m_window.reset();
}

void dust::Application::update()
{ }

void dust::Application::render()
{ }

void dust::Application::run()
{
    while(!m_window->shouldClose())
    {
        m_window->flush();
        m_time.delta = glfwGetTime() - m_time.time;
        m_time.time  = glfwGetTime();
        m_time.frame++;

        update();

        m_renderer->newFrame();
        {
            render();
        }
        m_renderer->endFrame();
    }
}

const dust::Window &
dust::Application::getWindow() const
{
    return *m_window;
}
const dust::Renderer &
dust::Application::getRenderer() const
{
    return *m_renderer;
}

dust::Application*
dust::Application::Get()
{
    return s_instance;
}

dust::Time
dust::Application::getTime() const {
    return m_time;
}