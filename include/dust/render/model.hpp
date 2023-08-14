#ifndef _DUST_RENDER_MODEL_HPP_
#define _DUST_RENDER_MODEL_HPP_

#include "dust/render/mesh.hpp"
#include "dust/render/shader.hpp"

namespace dust {
namespace render {

class Model
{
private:
    std::vector<Mesh*> m_meshes;
    glm::mat4 m_modelMat;

public:
        
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 tex;
        glm::vec3 normal;
        glm::vec4 color;
        // float texID;
    };

    Model(const std::vector<Mesh*> &mesh);
    ~Model();

    void draw(Shader *shader);    
};

}
}

#endif //_DUST_RENDER_MODEL_HPP_