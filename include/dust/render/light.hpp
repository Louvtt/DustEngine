#ifndef _DUST_RENDER_LIGHT_HPP_
#define _DUST_RENDER_LIGHT_HPP_

#include "dust/core/types.hpp"
#include "dust/render/camera.hpp"
#include "dust/render/framebuffer.hpp"

#include "glm/ext/vector_float3.hpp"

namespace dust {
namespace render {

class Shader;

class Light 
{
protected:
    // If their props has change (to rebake shadows)
    bool m_dirty;

    glm::mat4 m_proj;
    glm::mat4 m_view;
    glm::mat4 m_viewProj;
public:
    Light();
    virtual ~Light() = default;
    virtual void updateRenderPos() = 0;
    virtual void bind(ShaderPtr shader, u32 index = 0) const = 0;

    glm::mat4 getView() const;
    glm::mat4 getProj() const;
    glm::mat4 getViewProjMat(bool forceUpdate = false);
};


class DirectionnalLight
: public Light
{
protected:
    glm::vec3 m_direction;
    glm::vec3 m_color;

public:
    DirectionnalLight(glm::vec3 direction, glm::vec3 color);
    virtual ~DirectionnalLight();

    void updateRenderPos() override;
    virtual void bind(ShaderPtr shader, u32 index = 0) const override;

    glm::vec3 getDirection() const;
    glm::vec3 getColor() const;
    void setDirection(glm::vec3 direction);
    void setColor(glm::vec3 color);
};

}
}

#endif //_DUST_RENDER_LIGHT_HPP_