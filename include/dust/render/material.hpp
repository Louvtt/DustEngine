#ifndef _DUST_RENDER_MATERIAL_HPP_
#define _DUST_RENDER_MATERIAL_HPP_

#include "../core/types.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace dust {

namespace render {

/** @brief Max number of textures a material bind */
constexpr u32 MAX_MATERIAL_TEXTURE_COUNT = 5;

/**
 * @brief Base Material Class
 */
class Material {
protected:
    u32 m_boundSlot;
    std::string m_name;

    inline static Shader *s_shader = nullptr;

public:
    Material();
    virtual ~Material() = default;

    virtual void bind(u32 slot = 0) = 0;
    virtual void unbind()           = 0;

    void setName(const std::string &name);
    std::string getName() const;
};
using MaterialPtr  = Ref<Material>;
using MaterialUPtr = Scope<Material>;

class ColorMaterial : public Material {
public:
    glm::vec3 color;

public:
    ColorMaterial();
    ColorMaterial(glm::vec3 color);
    ~ColorMaterial() = default;

    void bind(u32 slot = 0) override;
    void unbind() override;
    static void SetupMaterialShader(Shader *shader);
};

class TextureMaterial : public Material {
public:
    TexturePtr texture;

public:
    TextureMaterial();
    ~TextureMaterial() = default;

    void bind(u32 slot = 0) override;
    void unbind() override;
    static void SetupMaterialShader(Shader *shader);
};

class PBRMaterial : public Material {
public:
    // Data
    // glm::vec3 albedo;
    // f32 roughness;
    // TexturePtr albedoTexture;
    // TexturePtr normalTexture;

    glm::vec3 albedo;
    f32 metallic;
    f32 roughness;
    f32 ao;

    TexturePtr albedoTexture;
    TexturePtr normalTexture;
    TexturePtr metallicTexture;
    TexturePtr roughnessTexture;
    TexturePtr aoTexture;

public:
    PBRMaterial();
    ~PBRMaterial() = default;

    void bind(u32 slot = 0) override;
    void unbind() override;
    static void SetupMaterialShader(Shader *shader);
};

}  // namespace render
}  // namespace dust

#endif  //_DUST_RENDER_MATERIAL_HPP_