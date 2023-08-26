#ifndef _DUST_RENDER_SKYBOX_HPP_
#define _DUST_RENDER_SKYBOX_HPP_

#include "../core/types.hpp"
#include "mesh.hpp"

#include <array>

namespace dust {
namespace render {

class Shader;
class Texture;
class Camera;

class Skybox
{
private:
    u32 m_renderID;
    Mesh *m_mesh;
    Scope<Shader> m_shader;

public:
    /**
     * @brief Create Skybox
     * @param skyboxTexturePaths path in order: Right/Left/Bottom/Top/Front/Back 
     */
    Skybox(std::array<std::string, 6> skyboxTexturePaths);
    ~Skybox();

    void draw(Camera* camera);
};

using SkyboxPtr = Ref<Skybox>;
using SkyboxUPtr = Scope<Skybox>;

}
}

#endif //_DUST_RENDER_SKYBOX_HPP_