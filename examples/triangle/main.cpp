#include "dust/dust.hpp"
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

class SimpleShaderApp
: public dust::Application
{
private:
    dust::render::Shader m_shader;
    dust::render::Mesh m_triangle;

public:
    SimpleShaderApp() 
    : dust::Application("Triangle"),
    m_shader(vCode, fCode),
    m_triangle(dust::render::MeshDescriptor{
        {   // pos               // color
            -0.5f, -0.5f, 0.f,   1.f, 0.f, 0.f, 1.f,
             0.5f, -0.5f, 0.f,   0.f, 1.f, 0.f, 1.f,
             0.0f,  0.5f, 0.f,   0.f, 0.f, 1.f, 1.f,
        }, 
        3,
        {},
        {
            dust::render::Attribute::Pos3D,
            dust::render::Attribute::Color
        }
    })
    {  }

    void render() override 
    {
        m_shader.use();
        m_triangle.draw();
    }
};

DUST_SIMPLE_ENTRY(SimpleShaderApp)

