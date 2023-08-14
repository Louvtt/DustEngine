#include "dust/core/layer.hpp"
#include "imgui.h"

dust::Layer::Layer(std::string name)
: m_name(name)
{ }

std::string dust::Layer::getName() const
{
    return m_name;
}

// DEBUG LAYER

dust::DebugLayer::DebugLayer()
: Layer("debug") {}

dust::DebugLayer::~DebugLayer()
{}

void dust::DebugLayer::update() 
{ }
void dust::DebugLayer::preRender() 
{
    ImGui::Begin("Debug");
}
void dust::DebugLayer::render() {}
void dust::DebugLayer::postRender()
{
    ImGui::End();
}