#include "dust/render/skybox.hpp"
#include "dust/core/application.hpp"
#include "dust/core/log.hpp"
#include "dust/core/profiling.hpp"
#include "dust/core/types.hpp"
#include "dust/io/assetsManager.hpp"
#include "dust/render/renderAPI.hpp"
#include "dust/render/texture.hpp"

#include "dust/render/mesh.hpp"
#include "dust/render/camera.hpp"
#include "dust/render/shader.hpp"
#include "glm/ext/vector_float3.hpp"

#include <stb_image.h>

namespace dr = dust::render;

constexpr static const char* vCode = SHADER_SOURCE(
    "#version 460 core",
    layout (location = 0) in vec3 aPos;
    out vec3 TexCoords;

    uniform mat4 uProj;
    uniform mat4 uView;

    void main()
    {
        TexCoords = aPos;
        // TexCoords.y = -TexCoords.y; // flip y
        vec4 pos = uProj * uView * vec4(aPos, 1.0);
        gl_Position = pos.xyww;
    }     
);
std::vector<dr::Attribute> CUBE_ATTRIBUTES { dr::Attribute::Pos3D };

constexpr static const char* fCode = SHADER_SOURCE(
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
m_mesh(dr::Mesh::createCube({2.f, 2.f, 2.f})),
m_shader(dust::createRef<Shader>(vCode, fCode))
{
    DUST_PROFILE_SECTION("Skybox::Constructor");
    io::Path assetsDirPath = io::AssetsManager::GetAssetsDir();
    glGenTextures(1, &m_renderID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderID);
    // load each side
    u32 index = 0;
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);
    for(auto path : skyboxTexturePaths)
    {
        io::Path imagePath = assetsDirPath / path;
        u8 *data = stbi_load(imagePath.string().c_str(), &width, &height, &nrChannels, 0);
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

    m_shader->setUniform("uSkybox", 0);

    // Generate cube
    DUST_DEBUG("[Skybox] Created Skybox {}", m_renderID);
}
dr::Skybox::~Skybox()
{
    DUST_PROFILE;
    glDeleteTextures(1, &m_renderID);
    m_mesh.reset();
    m_shader.reset();
}

void dr::Skybox::draw(Camera* camera)
{
    DUST_PROFILE_GPU("Skybox draw");
    // disable depth
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);

    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(camera->getView()));
    m_shader->setUniform("uProj", camera->getProj());
    m_shader->setUniform("uView", viewNoTranslation);

    // bind cubemap
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderID);

    m_mesh->draw(m_shader.get());
 
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
}