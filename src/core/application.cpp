#include "dust/core/application.hpp"
#include "dust/core/types.hpp"
#include "dust/core/log.hpp"

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
    
    s_instance = dust::Ref<dust::Application>(this);
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
        m_window->flush();

        update();
        render();

    }
}

dust::Weak<dust::Application> 
dust::Application::Get()
{
    return s_instance;
}