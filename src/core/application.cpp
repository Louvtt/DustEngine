#include "mx/core/application.hpp"
#include "mx/core/types.hpp"

#include <memory>

mx::Application::Application(const string& name, u16 width, u16 height)
: m_name(name),
m_time()
{ 
    m_window = mx::createScope<mx::Window>(name, width, height);
}

mx::Application::~Application()
{
    m_window.reset();
}

void mx::Application::update()
{ }

void mx::Application::render()
{ }

void mx::Application::run()
{
    while(!m_window->shouldClose())
    {
        m_window->beginFrame();

        update();
        render();

        m_window->endFrame();
    }
}