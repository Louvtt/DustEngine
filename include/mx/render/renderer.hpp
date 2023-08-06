#ifndef _MX_RENDER_RENDERER_HPP_
#define _MX_RENDER_RENDERER_HPP_

#include "../core/types.hpp"

class Renderer
{
private:
    string m_renderApiVersion;
    string m_renderApiName;

    bool m_initialized{false};

public:
    Renderer();
    ~Renderer();
};

#endif //_MX_RENDER_RENDERER_HPP_