#include "dust/render/skybox.hpp"
#include "dust/core/application.hpp"
#include "dust/core/log.hpp"
#include "dust/core/types.hpp"
#include "dust/render/renderAPI.hpp"
#include "dust/render/texture.hpp"

#include "dust/render/mesh.hpp"
#include "dust/render/camera.hpp"
#include "dust/render/shader.hpp"
#include "glm/ext/vector_float3.hpp"

#include <stb_image.h>

namespace dr = dust::render;

inline static constexpr float CUBE_VERTICES[] {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
     1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
     1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
     1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
     1.0f,-1.0f, 1.0f
};

std::string vCode = SHADER_SOURCE(
    "#version 460 core",
    layout (location = 0) in vec3 aPos;
    out vec3 TexCoords;

    uniform mat4 uProj;
    uniform mat4 uView;

    void main()
    {
        TexCoords = aPos;
        TexCoords.y = -TexCoords.y; // flip y
        vec4 pos = uProj * uView * vec4(aPos, 1.0);
        gl_Position = pos.xyww;
    }     
);
std::vector<dr::Attribute> CUBE_ATTRIBUTES { dr::Attribute::Pos3D };

std::string fCode = SHADER_SOURCE(
    "#version 460 core",
    out vec4 FragColor;

    in vec3 TexCoords;
    uniform samplerCube uSkybox;

    void main() {
        FragColor = texture(uSkybox, TexCoords);
    }
);

dr::Skybox::Skybox(std::array<std::string, 6> skyboxTexturePaths)
: m_renderID(0),
m_mesh(nullptr),
m_shader(dust::createScope<Shader>(vCode, fCode))
{
    glGenTextures(1, &m_renderID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderID);
    // load each side
    u32 index = 0;
    int width, height, nrChannels;
    for(auto path : skyboxTexturePaths)
    {
        u8 *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data != nullptr) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, 
                         0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
            );
        }
        else {
            DUST_ERROR("[Skybox] Cubemap tex failed to load {}", path);
        }
        stbi_image_free(data);
        ++index;
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // Generate cube
    auto m = new dr::Mesh(
        (void*)&CUBE_VERTICES[0], sizeof(float) * 3, (6 * 2 * 3), { dr::Attribute::Float3 }
    );
    m_mesh = dust::Scope<dr::Mesh>(m);

    DUST_DEBUG("[Skybox] Created Skybox {}", m_renderID);
}
dr::Skybox::~Skybox()
{
    glDeleteTextures(1, &m_renderID);

    m_shader.reset();
    m_mesh.reset();
}

void dr::Skybox::draw(Camera* camera)
{
    // disable depth
    glDepthFunc(GL_LEQUAL);

    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(camera->getView()));
    m_shader->setUniform("uProj", camera->getProj());
    m_shader->setUniform("uView", viewNoTranslation);

    // bind cubemap
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderID);
    m_shader->setUniform("uSkybox", 0);

    m_mesh->draw(m_shader.get());
 
    glDepthFunc(GL_LESS);
}