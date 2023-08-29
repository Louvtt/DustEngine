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

    glm::vec3 m_position;
public:
        
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 tex;
        glm::vec3 normal;
        glm::vec4 color;
        // float texID;
    };

    Model(const std::vector<Mesh*> &mesh);
    Model(Mesh* mesh);
    ~Model();

    void setPosition(glm::vec3 position);
    glm::vec3 getPosition() const;

    void draw(Shader *shader);    
};

using ModelPtr  = Ref<Model>;
using ModelUPtr = Scope<Model>; 

}
}

#endif //_DUST_RENDER_MODEL_HPP_