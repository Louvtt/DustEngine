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
    
    inline static Shader* s_shader = nullptr;
public:
    Material();
    virtual ~Material() = default;

    virtual void bind(u32 slot = 0) = 0;
    virtual void unbind() = 0;

    static void SetupMaterialShader(Shader* shader);
};
using MaterialPtr  = Ref<Material>;
using MaterialUPtr = Scope<Material>;


class ColorMaterial
: public Material
{
public:
    glm::vec3 color;

public:
    ColorMaterial();
    ColorMaterial(glm::vec3 color);
    ~ColorMaterial() = default;

    void bind(u32 slot = 0) override;
    void unbind() override;
    static void SetupMaterialShader(Shader* shader);
};

class TextureMaterial
: public Material
{
public:
    TexturePtr texture;

public:
    TextureMaterial();
    ~TextureMaterial() = default;

    void bind(u32 slot = 0) override;
    void unbind() override;
    static void SetupMaterialShader(Shader* shader);
};

class PBRMaterial
: public Material
{
public:
    // Data
    // glm::vec3 albedo;
    // f32 roughness;
    // TexturePtr albedoTexture;
    // TexturePtr metallicTexture;
    // TexturePtr roughnessTexture;
    // TexturePtr normalTexture;

    glm::vec3 albedo;
    TexturePtr albedoTexture;
    TexturePtr emissivityTexture;
    TexturePtr normalTexture;
    f32 metallic;
    f32 roughness;
    // TexturePtr roughnessTexture;
    glm::vec3 ior;
    TexturePtr reflectanceTexture;

public:
    PBRMaterial();
    ~PBRMaterial() = default;

    void bind(u32 slot = 0) override;
    void unbind() override;
    static void SetupMaterialShader(Shader* shader);
};

}
}

#endif //_DUST_RENDER_MATERIAL_HPP_