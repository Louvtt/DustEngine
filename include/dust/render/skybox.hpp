#ifndef _DUST_RENDER_SKYBOX_HPP_
#define _DUST_RENDER_SKYBOX_HPP_

#include "../core/types.hpp"

namespace dust {
namespace render {

class Shader;
class Mesh;
class Texture;

class Skybox
{
private:
    u32 m_renderID;
    Mesh* m_mesh;
    Shader* m_shader;

public:
    Skybox(const char* skyboxTexturePath);
    ~Skybox();

    void draw();
};

}
}

#endif //_DUST_RENDER_SKYBOX_HPP_