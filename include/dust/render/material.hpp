#ifndef _DUST_RENDER_MATERIAL_HPP_
#define _DUST_RENDER_MATERIAL_HPP_

#include "../core/types.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"

namespace dust {

namespace render {

/** @brief Max number of textures a material bind */
constexpr u32 MAX_MATERIAL_TEXTURE_COUNT = 4;

/**
 * @brief Base Material Class
 */
class Material
{
protected:
    u32 m_boundSlot;

public:
    Material();
    virtual ~Material() = default;

    virtual void bind(ShaderPtr shader, u32 slot = 0) = 0;
    virtual void unbind(ShaderPtr shader) = 0;
};
using MaterialPtr  = Ref<Material>;
using MaterialUPtr = Scope<Material>;


class ColorMaterial
: public Material
{
public:
    ColorMaterial(glm::vec3 color);
    ~ColorMaterial() = default;

    void bind(ShaderPtr shader, u32 slot = 0) override;
    void unbind(ShaderPtr shader) override;

    glm::vec3 color;
};

class TextureMaterial
: public Material
{
public:
    TextureMaterial();
    ~TextureMaterial() = default;

    void bind(ShaderPtr shader, u32 slot = 0) override;
    void unbind(ShaderPtr shader) override;

    TexturePtr texture;
};

class PBRMaterial
: public Material
{
public:
    PBRMaterial();
    ~PBRMaterial() = default;

    void bind(ShaderPtr shader, u32 slot = 0) override;
    void unbind(ShaderPtr shader) override;

    // Data

    /*
    glm::vec3 albedo;
    f32 roughness;

    TexturePtr albedoTexture;
    TexturePtr metallicTexture;
    TexturePtr roughnessTexture;
    TexturePtr normalTexture;
    */

    glm::vec3 albedo;
    TexturePtr albedoTexture;
    TexturePtr emissivityTexture;
    TexturePtr normalTexture;
    f32 metallic;
    f32 roughness;
    // TexturePtr roughnessTexture;
    glm::vec3 ior;
    TexturePtr reflectanceTexture;
};

}
}

#endif //_DUST_RENDER_MATERIAL_HPP_