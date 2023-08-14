#include "dust/render/camera.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_projection.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"

namespace dr = dust::render;

dr::Camera::Camera()
: m_far(1000), m_near(0),
m_proj(1.f), m_view(1.f)
{ }

// CAMERA 2D

dr::Camera2D::Camera2D(u32 width, u32 height, f32 far, f32 near)
: Camera(),
m_position(0.f, 0.f),
m_rotation(0.f)
{
    const f32 halfWidth = width * .5f;
    const f32 halfHeight = width * .5f;
    m_proj = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, near, far);
    m_view = glm::mat4(1.f);

    m_far = far;
    m_near = near;
}
void dr::Camera2D::bind(Shader *shader) 
{
    shader->setUniform("uView", m_view);
    shader->setUniform("uProj", m_proj);
}
void dr::Camera2D::resize(u32 width, u32 height) 
{
    const f32 halfWidth = width * .5f;
    const f32 halfHeight = width * .5f;
    m_proj = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, m_near, m_far);
}

void dr::Camera2D::move(glm::vec2 translation)
{
    m_position += translation;
    updateViewMatrix();
}
void dr::Camera2D::setPosition(glm::vec2 position)
{
    m_position = position;
    updateViewMatrix();
}

void dr::Camera2D::rotate(f32 angle)
{
    m_rotation += angle;
    updateViewMatrix();
}
void dr::Camera2D::setRotation(f32 rotation)
{
    m_rotation = rotation;
    updateViewMatrix();
}

void dr::Camera2D::updateViewMatrix() 
{
    m_view = glm::translate(
        glm::rotate(glm::mat4(1.f), glm::radians(m_rotation), glm::vec3(0.f, 0.f, 1.f)),
        glm::vec3(m_position.x, m_position.y, 0.f)
    );
}


//////////////////////////
// CAMERA 3D

dr::Camera3D::Camera3D(u32 width, u32 height, f32 fov, f32 far, f32 near)
: Camera(),
m_position(0.f, 0.f, 0.f),
m_forward(0.f, 0.f, -1.f),
m_up(0.f, 1.f, 0.f),
m_fov(fov),
m_aspectRatio((f32)width / (f32)height)
{
    m_proj = glm::perspective(glm::radians(m_fov), m_aspectRatio, near, far);
    m_view = glm::mat4(1.f);
    m_far = far;
    m_near = near;
}

void dr::Camera3D::bind(Shader *shader) 
{
    shader->setUniform("uView", m_view);
    shader->setUniform("uProj", m_proj);
}
void dr::Camera3D::resize(u32 width, u32 height) 
{
    m_aspectRatio = (f32)width / (f32)height;
    m_proj = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_near, m_far);
}

void dr::Camera3D::move(glm::vec3 translation)
{
    m_position += translation;
    updateViewMatrix();
}
void dr::Camera3D::setPosition(glm::vec3 position)
{
    m_position = position;
    updateViewMatrix();
}

void dr::Camera3D::rotate(glm::vec3 angle)
{
    m_rotation += angle;
    m_forward.x = cos(glm::radians(m_rotation.x)) * cos(glm::radians(m_rotation.y));
    m_forward.y = sin(glm::radians(m_rotation.y));
    m_forward.z = sin(glm::radians(m_rotation.x)) * cos(glm::radians(m_rotation.y));
    updateViewMatrix();
}
void dr::Camera3D::setRotation(glm::vec3 rotation)
{
    m_rotation  = rotation;
    m_forward.x = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
    m_forward.y = sin(glm::radians(rotation.y));
    m_forward.z = sin(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
    updateViewMatrix();
}

void dr::Camera3D::updateViewMatrix() 
{
    m_view = glm::lookAt(m_position, m_position + m_forward, m_up);
}

glm::vec3 dr::Camera3D::forward() const
{
    return m_forward;
}