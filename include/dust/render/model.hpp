#ifndef _DUST_RENDER_MODEL_HPP_
#define _DUST_RENDER_MODEL_HPP_

#include "../render/mesh.hpp"
#include "../render/shader.hpp"

namespace dust {
namespace render {

struct ModelVertex {
    glm::vec3 pos;
    glm::vec2 tex;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec4 color;
    float materialID;
};

class Model {
private:
    std::vector<MeshPtr> m_meshes;
    glm::mat4 m_modelMat;

    glm::vec3 m_position;

public:
    Model(const std::vector<MeshPtr> &mesh);
    Model(MeshPtr mesh);
    ~Model();

    void setPosition(glm::vec3 position);
    glm::vec3 getPosition() const;
    std::vector<MeshPtr> getMeshes() const;

    void draw(Shader *shader);
};

using ModelPtr  = Ref<Model>;
using ModelUPtr = Scope<Model>;

}  // namespace render
}  // namespace dust

#endif  //_DUST_RENDER_MODEL_HPP_