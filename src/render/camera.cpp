#include "dust/render/camera.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_projection.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include <climits>

namespace dr = dust::render;

dr::Camera::Camera()
: m_far(1000), m_near(0),
m_proj(1.f), m_view(1.f)
{ 
    if(s_activeCamera == nullptr) s_activeCamera = this;
}

void dr::Camera::makeActive()
{
    s_activeCamera = this;   
}
dr::Camera* dr::Camera::GetActive()
{
    return s_activeCamera;
}

glm::mat4 dr::Camera::getView() const
{
    return m_view;
}
glm::mat4 dr::Camera::getProj() const
{
    return m_proj;
}

void dr::Camera::setProj(glm::mat4 proj)
{
    m_proj = proj;
}

void dr::Camera::setView(glm::mat4 view)
{
    m_view = view;
}

[[nodiscard]]
dr::CameraFrustrum dr::Camera::getFrustrum() const {
    const auto inv = glm::inverse(m_proj * m_view);
    dr::CameraFrustrum res{};

    res.farTopLeft       = inv * glm::vec4(-1.f,  1.f, 1.f, 1.f); 
    res.farTopLeft     /= res.farTopLeft.w;
    res.farTopRight      = inv * glm::vec4( 1.f,  1.f, 1.f, 1.f); 
    res.farTopRight    /= res.farTopRight.w;
    res.farBottomLeft    = inv * glm::vec4(-1.f, -1.f, 1.f, 1.f); 
    res.farBottomLeft  /= res.farBottomLeft.w;
    res.farBottomRight   = inv * glm::vec4( 1.f, -1.f, 1.f, 1.f); 
    res.farBottomRight /= res.farBottomRight.w;

    res.nearTopLeft     = inv * glm::vec4(-1.f,  1.f, -1.f, 1.f); 
    res.nearTopLeft     /= res.nearTopLeft.w;
    res.nearTopRight    = inv * glm::vec4( 1.f,  1.f, -1.f, 1.f); 
    res.nearTopRight    /= res.nearTopRight.w;
    res.nearBottomLeft  = inv * glm::vec4(-1.f, -1.f, -1.f, 1.f); 
    res.nearBottomLeft  /= res.nearBottomLeft.w;
    res.nearBottomRight = inv * glm::vec4( 1.f, -1.f, -1.f, 1.f); 
    res.nearBottomRight /= res.nearBottomRight.w;

    return res;
}

// CAMERA 2D

dr::Camera2D::Camera2D(u32 width, u32 height, f32 far, f32 near)
: Camera(),
m_position(0.f, 0.f),
m_rotation(0.f),
m_size(width, height)
{
    const f32 halfWidth = width * .5f;
    const f32 halfHeight = height * .5f;
    m_proj = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, near, far);
    m_view = glm::mat4(1.f);

    m_far = far;
    m_near = near;
    updateViewMatrix();
}

void dr::Camera2D::bind(Shader *shader) 
{
    shader->setUniform("uView", m_view);
    shader->setUniform("uProj", m_proj);
}
void dr::Camera2D::resize(u32 width, u32 height) 
{
    const f32 halfWidth = width * .5f;
    const f32 halfHeight = height * .5f;
    m_proj = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, m_near, m_far);
    m_size = glm::vec2(width, height);
}

void dr::Camera2D::move(glm::vec2 translation)
{
    m_position += translation;
    updateViewMatrix();
}
void dr::Camera2D::move(glm::vec3 translation)
{
    m_position.x += translation.x;
    m_position.y += translation.y;
    updateViewMatrix();
}
void dr::Camera2D::setPosition(glm::vec2 position)
{
    m_position = position;
    updateViewMatrix();
}

// [[nodiscard]]
// dr::CameraFrustrum dr::Camera2D::getFrustrum() const {
//     const glm::vec2 half = m_size * .5f;
//     dr::CameraFrustrum res{};
//     res.nearTopRight    = {m_position.x + half.x, m_position.y + half.y, m_near};
//     res.nearTopLeft     = {m_position.x - half.x, m_position.y + half.y, m_near};
//     res.nearBottomRight = {m_position.x + half.x, m_position.y - half.y, m_near};
//     res.nearBottomLeft  = {m_position.x - half.x, m_position.y - half.y, m_near};

//     res.farTopRight    = {m_position.x + half.x, m_position.y + half.y, m_far};
//     res.farTopLeft     = {m_position.x - half.x, m_position.y + half.y, m_far};
//     res.farBottomRight = {m_position.x + half.x, m_position.y - half.y, m_far};
//     res.farBottomLeft  = {m_position.x - half.x, m_position.y - half.y, m_far};
//     return res;
// }

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
m_rotation(0.f, 0.f, 180.f),
m_forward(0.f, 0.f, -1.f),
m_up(0.f, 1.f, 0.f),
m_fov(fov),
m_aspectRatio((f32)width / (f32)height)
{
    m_proj = glm::perspective(glm::radians(m_fov), m_aspectRatio, near, far);
    m_view = glm::mat4(1.f);
    m_far = far;
    m_near = near;

    updateViewMatrix();
}

void dr::Camera3D::bind(Shader *shader) 
{
    shader->setUniform("uView", m_view);
    shader->setUniform("uViewPos", m_position);
    shader->setUniform("uProj", m_proj);
}
void dr::Camera3D::resize(u32 width, u32 height) 
{
    m_aspectRatio = (f32)width / (f32)height;
    m_proj = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_near, m_far);
}

void dr::Camera3D::move(glm::vec2 translation)
{
    m_position.x += translation.x;
    m_position.y += translation.y;
    updateViewMatrix();
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

void dr::Camera3D::lookAt(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
    m_position = position;
    m_forward = glm::normalize(position - target);
    m_up = up;
    updateViewMatrix();
}

void dr::Camera3D::updateViewMatrix() 
{
    m_view = glm::lookAt(m_position, m_position - m_forward, m_up);
}

glm::vec3 dr::Camera3D::forward() const
{
    return m_forward;
}

// Source: http://www.lighthouse3d.com/tutorials/view-frustum-culling/view-frustums-shape/
// [[nodiscard]]
// dr::CameraFrustrum dr::Camera3D::getFrustrum() const
// {
//     // TODO: verify the order
//     const glm::vec3 right = glm::cross(m_up, m_forward);

//     // center of near and far plane
//     const glm::vec3 nearCenter = m_position - (m_forward * m_near);
//     const glm::vec3 farCenter  = m_position - (m_forward * m_far);

//     // Width and Height of the planes
//     const f32 fovRadians = glm::radians(m_fov);
//     const f32 nearHeight = 2 * tan(fovRadians/ 2) * m_near;
//     const f32 nearWidth  = nearHeight * m_aspectRatio;
//     const f32 farHeight  = 2 * tan(fovRadians / 2) * m_far;
//     const f32 farWidth   = farHeight * m_aspectRatio;

//     dr::CameraFrustrum res{};
//     res.nearTopLeft     = nearCenter + m_up * nearHeight - right * nearWidth;
//     res.nearTopRight    = nearCenter + m_up * nearHeight + right * nearWidth;
//     res.nearBottomLeft  = nearCenter - m_up * nearHeight - right * nearWidth;
//     res.nearBottomRight = nearCenter - m_up * nearHeight + right * nearWidth;

//     res.farTopLeft     = farCenter + m_up * farHeight - right * farWidth;
//     res.farTopRight    = farCenter + m_up * farHeight + right * farWidth;
//     res.farBottomLeft  = farCenter - m_up * farHeight - right * farWidth;
//     res.farBottomRight = farCenter - m_up * farHeight + right * farWidth;
//     return res;
// }