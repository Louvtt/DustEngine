#ifndef _DUST_RENDER_MATERIAL_HPP_
#define _DUST_RENDER_MATERIAL_HPP_

#include "../core/types.hpp"
#include "glm/ext/vector_float4.hpp"

namespace dust {

namespace render {

class Shader;
class Texture;

class Material
{
public:
    Material();
    virtual ~Material() = default;

    virtual void bind(Shader *shader) = 0;
    virtual void unbind(Shader *shader) = 0;
};


class TextureMaterial
: public Material
{
protected:
    Texture* m_texture;

public:
    TextureMaterial(Texture* texture);
    ~TextureMaterial();

    void bind(Shader *shader) override;
    void unbind(Shader *shader) override;
};

class PBRMaterial
: public Material
{
protected:
    Texture* m_diffuse;
    glm::vec4 m_ambientColor;
    glm::vec4 m_diffuseColor;

public:
    PBRMaterial(Texture* diffuseTexture = nullptr, glm::vec4 ambient = glm::vec4(1.f), glm::vec4 diffuse = glm::vec4(1.f));
    ~PBRMaterial();

    void bind(Shader *shader) override;
    void unbind(Shader *shader) override;
};

}

}

#endif //_DUST_RENDER_MATERIAL_HPP_