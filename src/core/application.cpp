#include "dust/core/application.hpp"
#include "dust/core/layer.hpp"
#include "dust/core/types.hpp"
#include "dust/core/log.hpp"
#include "dust/io/inputManager.hpp"
#include "dust/render/renderer.hpp"

#include "imgui.h"

#include <GLFW/glfw3.h>

#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include <memory>

#include "tracy/Tracy.hpp"

dust::Application::Application(const std::string& name, u32 width, u32 height)
: m_name(name),
m_time(),
m_layers()
{
    if(s_instance) {
        DUST_ERROR("Cannot create another application instance");
        return;
    }

    #ifdef _DEBUG
    spdlog::set_level(spdlog::level::debug);
    #endif

    // ImGui init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    m_window = dust::createScope<dust::Window>(name, width, height);
    m_inputManager = dust::createScope<dust::InputManager>(*m_window);
    m_renderer = dust::createScope<dust::Renderer>(*m_window);

    s_instance = this;

    // first to draw context
    {
        m_window->flush();
        m_renderer->setClearColor(0.1f, 0.1f, 0.1f, 1.f);
        m_renderer->newFrame();
        ImGui::NewFrame();
        ImGui::Text("Loading...");
        ImGui::GetForegroundDrawList()->AddText(ImVec2(0, 0), IM_COL32(255, 255, 255, 255), "Loading...");
        ImGui::Render();
        m_renderer->endFrame();
        m_window->swapBuffers();
    }
}

dust::Application::~Application()
{
    m_inputManager.reset();
    m_renderer.reset();
    m_window.reset();
}

void dust::Application::update()
{ 
    ZoneScoped;
}

void dust::Application::render()
{ 
    ZoneScoped;
}

void dust::Application::run()
{
    while(!m_window->shouldClose())
    {
        m_window->flush();
        m_time.delta = glfwGetTime() - m_time.time;
        m_time.time  = glfwGetTime();
        m_time.frame++;

        update();
        for(auto [_, layer] : m_layers) { layer->update(); }
        m_inputManager->updateState();

        m_renderer->newFrame();
        ImGui::NewFrame();
        for(auto [_, layer] : m_layers) { layer->preRender(); }
        {
            render();
            for(auto [_, layer] : m_layers) { layer->render(); }
        }
        for(auto [_, layer] : m_layers) { layer->postRender(); }
        ImGui::Render();
        m_renderer->endFrame();
    }
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
    m_layers.insert({
        layer->getName(),
        layer
    });
}
void dust::Application::popLayer(std::string name)
{
    auto found = m_layers.find(name);
    if(found == m_layers.end()) return; // not found
    auto deletedLayer = m_layers.erase(found);
    delete deletedLayer->second;
}