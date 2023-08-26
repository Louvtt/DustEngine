#ifndef _DUST_RENDER_MATERIAL_HPP_
#define _DUST_RENDER_MATERIAL_HPP_

#include "../core/types.hpp"
#include "glm/ext/vector_float3.hpp"
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

class ColorMaterial
: public Material
{
protected:
    glm::vec3 m_color;

public:
    ColorMaterial(glm::vec3 color);
    ~ColorMaterial() = default;

    void bind(Shader *shader) override;
    void unbind(Shader *shader) override;
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
public:
    struct Data {
        Texture* diffuse;

        glm::vec4 ambientColor;
        glm::vec4 diffuseColor;
        glm::vec4 specularColor;
        float shininess;
    };
protected:
    Data m_data;

public:
    PBRMaterial(const Data& data);
    ~PBRMaterial();

    void bind(Shader *shader) override;
    void unbind(Shader *shader) override;
};

}

}

#endif //_DUST_RENDER_MATERIAL_HPP_