#ifndef _DUST_RENDER_RENDERER_HPP_
#define _DUST_RENDER_RENDERER_HPP_

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

#endif //_DUST_RENDER_RENDERER_HPP_