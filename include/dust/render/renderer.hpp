#ifndef _DUST_RENDER_RENDERER_HPP_
#define _DUST_RENDER_RENDERER_HPP_

#include "../core/types.hpp"
#include "../core/window.hpp"

#include <glm/vec4.hpp>

namespace dust {

class Renderer
{
private:
    std::string m_renderApiVersion;
    std::string m_renderApiName;

    bool m_initialized{false};

    bool m_depthEnabled{true};
public:
    Renderer(const Window& window);
    ~Renderer();

    virtual void newFrame(); // clear buffer
    virtual void endFrame(); // render

    virtual void setClearColor(float r, float g, float b, float a = 1.f);
    void setClearColor(glm::vec4 color);

    virtual void setDepthWrite(bool write);
    virtual void setDepthTest(bool test);

    virtual void resize(u32 width, u32 height);
};

}

#endif //_DUST_RENDER_RENDERER_HPP_