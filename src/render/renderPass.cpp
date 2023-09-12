#include "dust/render/renderPass.hpp"

namespace dust::render {

RenderPass::RenderPass(const RenderPassDesc &desc)
    : m_framebuffer(desc.framebuffer), m_shader(desc.shader) {
}

void RenderPass::preRender() {
    m_framebuffer->bind();
}

void RenderPass::postRender() {
    m_framebuffer->unbind();
}

Shader *RenderPass::getShader() const {
    return m_shader.get();
}
Framebuffer *RenderPass::getFramebuffer() const {
    return m_framebuffer.get();
}

/********************************************************/
const MeshPtr PostProcessPass::s_screenQuad =
    Mesh::createPlane(glm::vec2{1.f}, true);

PostProcessPass::PostProcessPass(const RenderPassDesc &desc)
    : RenderPass(desc) {
}

void PostProcessPass::preRender() {
    m_framebuffer->bind();
    s_screenQuad->draw(m_shader.get());
}
void PostProcessPass::postRender() {
    m_framebuffer->unbind();
}

} // namespace dust::render