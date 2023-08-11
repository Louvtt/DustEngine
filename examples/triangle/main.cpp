#include "dust/dust.hpp"
#include "dust/render/shader.hpp"

std::string vCode = SHADER_SOURCE(
    "#version 460",
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoords;
    layout (location = 2) in vec3 aNormals;
    layout (location = 3) in vec4 aColor;

    out vec4 oColor;

    uniform float uTime;
    uniform mat4 MVP;

    void main() {
        gl_Position = MVP * vec4(aPos, 1.f);
        oColor = aColor;
    }
);

std::string fCode = SHADER_SOURCE(
    "#version 460",
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
public:
    SimpleShaderApp() 
    : dust::Application("Triangle"),
    m_shader(dust::render::Shader(vCode, fCode))
    {  }

    void render() override 
    {
        m_shader.use();
    }
};

DUST_SIMPLE_ENTRY(SimpleShaderApp)

