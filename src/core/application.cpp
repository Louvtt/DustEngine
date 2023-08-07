#include "dust/core/application.hpp"
#include "dust/core/types.hpp"

#include <memory>

dust::Application::Application(const string& name, u16 width, u16 height)
: m_name(name),
m_time()
{
    m_window = dust::createScope<dust::Window>(name, width, height);
}

dust::Application::~Application()
{
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
        m_window->beginFrame();

        update();
        render();

        m_window->endFrame();
    }
}