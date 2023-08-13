#ifndef _DUST_RENDER_CAMERA_HPP_
#define _DUST_RENDER_CAMERA_HPP_

#include "dust/core/types.hpp"
#include "dust/core/window.hpp"
#include "dust/render/shader.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/quaternion_float.hpp"

namespace dust {
namespace render {

class Camera
{
protected:
    glm::mat4 m_view;
    glm::mat4 m_proj;

    f32 m_near;
    f32 m_far;
protected:
    Camera();
    ~Camera() = default;

public:
    virtual void bind(Shader *shader) = 0;
    virtual void resize(u32 width, u32 height) = 0;

protected:
    virtual void updateViewMatrix() = 0;
};

class Camera2D
: public Camera
{
protected:
    glm::vec2 m_position;
    // degrees
    f32 m_rotation;

public:
    Camera2D(u32 width, u32 height, f32 far = 1000, f32 near = .1f);
    ~Camera2D() = default;

    void bind(Shader *shader) override;
    void resize(u32 width, u32 height) override;

    void move(glm::vec2 translation);
    void setPosition(glm::vec2 position);

    void rotate(f32 angle);
    void setRotation(f32 rotation);

private:
    void updateViewMatrix() override;
};


class Camera3D
: public Camera
{
protected:
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_forward;
    glm::vec3 m_up;

    f32 m_aspectRatio;
    f32 m_fov;
public:
    Camera3D(u32 width, u32 height, f32 fov, f32 far = 1000, f32 near = .1f);
    ~Camera3D() = default;

    void bind(Shader *shader) override;
    void resize(u32 width, u32 height) override;
    void setFOV(f32 fov);

    void move(glm::vec3 translation);
    void setPosition(glm::vec3 position);

    void rotate(glm::vec3 angle);
    void setRotation(glm::vec3 rotation);

private:
    void updateViewMatrix() override;
};

}
}

#endif //_DUST_RENDER_CAMERA_HPP_