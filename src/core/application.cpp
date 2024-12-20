#include "dust/core/application.hpp"
#include "dust/core/layer.hpp"
#include "dust/core/profiling.hpp"
#include "dust/core/types.hpp"
#include "dust/core/log.hpp"
#include "dust/io/inputManager.hpp"
#include "dust/render/renderer.hpp"
#include "dust/core/profiling.hpp"
#include "dust/editor/editor.hpp"

#include "imgui.h"

#include <GLFW/glfw3.h>

#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include <memory>

dust::Application::Application(const std::string& name, u32 width, u32 height)
: m_name(name),
m_time(),
m_layers()
{
    DUST_PROFILE_SECTION("Application::Constructor");
    if(s_instance) {
        DUST_ERROR("Cannot create another application instance");
        return;
    }

    #ifdef _DEBUG
    spdlog::set_level(spdlog::level::debug);
    #endif

    m_window = dust::createScope<dust::Window>(name, width, height);
    m_inputManager = dust::createScope<dust::InputManager>(*m_window);
    m_renderer = dust::createScope<dust::Renderer>(*m_window);
    m_resourceManager = dust::createScope<dust::io::ResourceManager>();
    m_editor = dust::createScope<dust::Editor>(m_window.get());

    s_instance = this;

    // first to draw context
    {
        m_window->flush();
        m_renderer->setClearColor(0.1f, 0.1f, 0.1f, 1.f);
        m_renderer->newFrame();
        m_editor->display_loading_frame();
        m_renderer->endFrame();
        m_window->swapBuffers();
    }
}

dust::Application::~Application()
{
    m_resourceManager.reset();
    m_editor.reset();
    m_inputManager.reset();
    m_renderer.reset();
    m_window.reset();
}

void dust::Application::update()
{ 
    DUST_PROFILE_SECTION("AppUpdate");
}

void dust::Application::render()
{ 
    DUST_PROFILE_SECTION("render");
}

void dust::Application::run()
{
    DUST_PROFILE;
    while(!m_window->shouldClose())
    {
        DUST_PROFILE_FRAME("main");
        m_window->flush();
        m_time.delta = glfwGetTime() - m_time.time;
        m_time.time  = glfwGetTime();
        m_time.frame++;

        update();
        for(auto [_, layer] : m_layers) { layer->update(); }
        m_inputManager->updateState();
        m_resourceManager->update();

        m_renderer->newFrame();
        m_editor->new_frame();
        for(auto [_, layer] : m_layers) { layer->preRender(); }
        {
            render();
            for(auto [_, layer] : m_layers) { layer->render(); }
        }
        for(auto [_, layer] : m_layers) { layer->postRender(); }
        m_editor->render_ui();
        m_renderer->endFrame();
    }
    DUST_INFO("Exitting app main loop.");
}

void dust::Application::setProgramPath(const std::filesystem::path& path)
{
    m_programPath = path;
}

dust::Window*
dust::Application::getWindow() const
{
    return m_window.get();
}
dust::Renderer*
dust::Application::getRenderer() const
{
    return m_renderer.get();
}
dust::InputManager*
dust::Application::getInputManager() const
{
    return m_inputManager.get();
}

dust::io::ResourceManager*
dust::Application::getResourceManager() const 
{
    return m_resourceManager.get();
}

dust::ScriptingManager*
dust::Application::getScriptingManager() const {
    return m_scriptingManager.get();
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

std::filesystem::path
dust::Application::getProgramPath() const
{
    return m_programPath;
}

void dust::Application::pushLayer(Layer* layer)
{
    DUST_PROFILE;
    m_layers.insert({
        layer->getName(),
        layer
    });
}
void dust::Application::popLayer(std::string name)
{
    DUST_PROFILE;
    auto found = m_layers.find(name);
    if(found == m_layers.end()) return; // not found
    auto deletedLayer = m_layers.erase(found);
    delete deletedLayer->second;
}
dust::Editor *dust::Application::getEditor() const { return m_editor.get(); }
