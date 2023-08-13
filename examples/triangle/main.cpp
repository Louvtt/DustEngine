#include "dust/core/log.hpp"
#include "dust/dust.hpp"
#include "dust/io/keycodes.hpp"
#include "dust/render/mesh.hpp"
#include "dust/render/shader.hpp"

std::string vCode = SHADER_SOURCE(
    "#version 460 core",
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec4 aColor;
    out vec4 oColor;
    void main() {
        gl_Position = vec4(aPos, 1.f);
        oColor = aColor;
    }
);

std::string fCode = SHADER_SOURCE(
    "#version 460 core",
    out vec4 fragColor;
    in vec4 oColor;
    void main() {
        fragColor = oColor;
    }
);

std::vector<float> triangleVertices{
    // pos               // color
    -0.5f, -0.5f, 0.f,   1.f, 0.f, 0.f, 1.f,
     0.5f, -0.5f, 0.f,   0.f, 1.f, 0.f, 1.f,
     0.0f,  0.5f, 0.f,   0.f, 0.f, 1.f, 1.f,
};

class SimpleShaderApp
: public dust::Application
{
private:
    dust::render::Shader m_shader;
    dust::render::Mesh m_triangle;

    bool m_polygonLineMode{false};

public:
    SimpleShaderApp() 
    : dust::Application("Triangle"),
    m_polygonLineMode(false),
    m_shader(vCode, fCode),
    m_triangle(triangleVertices, sizeof(float) * 8, 3, {
        dust::render::Attribute::Pos3D,
        dust::render::Attribute::Color
    })
    {  }

    void update() override
    {
        if(getInputManager()->isKey(dust::Key::P, dust::KeyState::Press))
        {
            m_polygonLineMode = !m_polygonLineMode;
            getRenderer()->setDrawWireframe(m_polygonLineMode);
        }
    }

    void render() override 
    {
        m_shader.use();
        m_triangle.draw();
    }
};

DUST_SIMPLE_ENTRY(SimpleShaderApp)

