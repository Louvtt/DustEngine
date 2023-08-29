#include "dust/render/light.hpp"
#include "dust/core/log.hpp"
#include "dust/core/types.hpp"
#include "dust/render/camera.hpp"
#include "dust/render/shader.hpp"

#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"

namespace dr = dust::render;

dr::Light::Light()
: m_proj(1.f),
m_view(1.f) 
{ }

glm::mat4 dr::Light::getView() const
{
    return m_view;
}
glm::mat4 dr::Light::getProj() const
{
    return m_proj;
}

glm::mat4 dr::Light::getViewProjMat(bool forceUpdate)
{
    if(m_dirty || forceUpdate) {
        updateRenderPos();
        m_dirty = false;
    }
    return m_viewProj;
}


dr::DirectionnalLight::DirectionnalLight(glm::vec3 direction, glm::vec3 color)
: Light(),
m_color(color),
m_direction(direction) 
{
    updateRenderPos();
}

dr::DirectionnalLight::~DirectionnalLight() {}


void dr::DirectionnalLight::bind(Shader* shader) const
{
    shader->setUniform("uLightDir", m_direction);
    shader->setUniform("uLightColor", m_color);
}

glm::vec3 dr::DirectionnalLight::getDirection() const
{
    return m_direction;
}
glm::vec3 dr::DirectionnalLight::getColor() const
{
    return m_color;
}
void dr::DirectionnalLight::setDirection(glm::vec3 direction)
{
    m_direction = direction;
    m_dirty = true; 
}
void dr::DirectionnalLight::setColor(glm::vec3 color)
{
    m_color = color;
}

void dr::DirectionnalLight::updateRenderPos()
{
    const auto camera = Camera::GetActive();
    if(!camera) return;
    const dr::CameraFrustrum frustrum = camera->getFrustrum();
    // calculate bounding box

    // calculate view matrix
    {
        glm::vec3 center = glm::vec3(0, 0, 0);
        // const glm::vec4* it = &frustrum.nearTopRight;
        // for(u16 i = 0; i < 8; ++i, it++)
        // {
        //     center += glm::vec3(*it);
        // }
        // center *= 0.125f; // divide by 8

        // update ligth view
        m_view = glm::lookAt(
            center + (m_direction * 10.f),
            center,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
    }

    // calculate projection matrix
    {
        // f32 minX = std::numeric_limits<f32>::max();
        // f32 maxX = std::numeric_limits<f32>::lowest();
        // f32 minY = std::numeric_limits<f32>::max();
        // f32 maxY = std::numeric_limits<f32>::lowest();
        // f32 minZ = std::numeric_limits<f32>::max();
        // f32 maxZ = std::numeric_limits<f32>::lowest();
        // const glm::vec4* it = &frustrum.nearTopRight;
        // for(u16 i = 0; i < 8; ++i, it++)
        // {
        //     const auto trf = m_view * (*it);
        //     minX = std::min(minX, trf.x);
        //     maxX = std::max(maxX, trf.x);
        //     minY = std::min(minY, trf.y);
        //     maxY = std::max(maxY, trf.y);
        //     minZ = std::min(minZ, trf.z);
        //     maxZ = std::max(maxZ, trf.z);
        // }

        // // Tune this parameter according to the scene
        // constexpr float zMult = 10.0f;
        // if (minZ < 0) minZ *= zMult;
        // else          minZ /= zMult;

        // if (maxZ < 0) maxZ /= zMult;
        // else          maxZ *= zMult;
        // m_proj = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
        m_proj = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 10.f);
    }

    m_viewProj = m_proj * m_view;
}