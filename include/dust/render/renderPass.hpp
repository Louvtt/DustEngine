#ifndef _DUST_RENDER_RENDERPASS_HPP_
#define _DUST_RENDER_RENDERPASS_HPP_

#include "dust/render/framebuffer.hpp"
#include "dust/render/mesh.hpp"
#include "dust/render/shader.hpp"

namespace dust::render {

struct RenderPassDesc {
    ShaderPtr shader;
    FramebufferPtr framebuffer;
    /* TODO: add a scene */
};

class RenderPass {
protected:
    ShaderPtr m_shader;
    FramebufferPtr m_framebuffer;

public:
    RenderPass(const RenderPassDesc &desc);
    ~RenderPass() = default;

    virtual void preRender();
    virtual void postRender();

    Shader *getShader() const;
    Framebuffer *getFramebuffer() const;
};

class PostProcessPass : public RenderPass {
private:
    static const MeshPtr s_screenQuad;

public:
    PostProcessPass(const RenderPassDesc &desc);
    ~PostProcessPass() = default;

    virtual void preRender() override;
    virtual void postRender() override;
};

} // namespace dust::render

#endif //_DUST_RENDER_RENDERPASS_HPP_