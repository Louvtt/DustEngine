#include "dust/render/material.hpp"

#include "dust/render/texture.hpp"
#include "dust/render/shader.hpp"
#include "dust/core/profiling.hpp"
#include "dust/render/mesh.hpp"
#include "glm/ext/vector_float4.hpp"
#include <format>


namespace dr = dust::render;

static std::string shaderMaterialLoc(u32 index) {
    DUST_PROFILE;
    return std::format("uMaterials[{}]", index);
}

/*********************************************************/

dr::Material::Material()
: m_boundSlot(0), m_name("Material") {}

void dr::Material::setName(const std::string &name)
{
    m_name = name;
}
std::string dr::Material::getName() const
{
    return m_name;
}

/*********************************************************/

dr::ColorMaterial::ColorMaterial(glm::vec3 color)
: color({1.f, 1.f, 1.f}) 
{ }
dr::ColorMaterial::ColorMaterial() 
: ColorMaterial({1.f, 1.f, 1.f}) { }


void dr::ColorMaterial::SetupMaterialShader(Shader *shader)
{ 
    s_shader = shader;
}
void dr::ColorMaterial::bind(u32 slot)   
{
    DUST_PROFILE;
    m_boundSlot = slot;
    const std::string loc = shaderMaterialLoc(slot);
    s_shader->setUniform(loc + ".exist", true);
    s_shader->setUniform(loc + ".diffuse", glm::vec4{color, 1.f});
}
void dr::ColorMaterial::unbind() 
{
    DUST_PROFILE;
    const std::string loc = shaderMaterialLoc(m_boundSlot);
    s_shader->setUniform(loc + ".exist", false);
}

/*********************************************************/

dr::TextureMaterial::TextureMaterial()
: texture(Texture::GetNullTexture()) { }
void dr::TextureMaterial::SetupMaterialShader(Shader* shader)
{ 
    s_shader = shader;
    for(int slot = 0; slot < DUST_MATERIAL_SLOTS; ++slot) {
        const std::string loc = shaderMaterialLoc(slot);
        s_shader->setUniform(loc + ".albedo", (int)(slot * MAX_MATERIAL_TEXTURE_COUNT));
    }
}
void dr::TextureMaterial::bind(u32 slot)
{
    DUST_PROFILE;
    m_boundSlot = slot;
    const std::string loc = shaderMaterialLoc(slot);
    s_shader->setUniform(loc + ".exist", true);
    texture->bind(0);
    s_shader->setUniform(loc + ".albedo", (int)(slot * MAX_MATERIAL_TEXTURE_COUNT));
}

void dr::TextureMaterial::unbind()
{
    DUST_PROFILE;
    const std::string loc = shaderMaterialLoc(m_boundSlot);
    texture->unbind();
    s_shader->setUniform("uHasMaterial", false);
    s_shader->setUniform("uMaterial.texture", (int)(m_boundSlot * MAX_MATERIAL_TEXTURE_COUNT));
}

/*********************************************************/

dr::PBRMaterial::PBRMaterial()
: albedo({1.f, 1.f, 1.f}),
ior(0.f),
roughness(0.f),
metallic(0.f),
albedoTexture(Texture::GetNullTexture()),
reflectanceTexture(Texture::GetNullTexture()),
emissivityTexture(Texture::GetNullTexture()),
normalTexture(Texture::GetNullTexture())
{  }

void dr::PBRMaterial::SetupMaterialShader(Shader *shader)
{
    DUST_PROFILE;
    s_shader = shader;
    static constexpr std::string albedoU      = "uMatAlbedo";
    static constexpr std::string emissiveU    = "uMatEmissive";
    static constexpr std::string reflectanceU = "uMatReflectance";
    static constexpr std::string normalU      = "uMatNormal";
    for(int slot = 0; slot < DUST_MATERIAL_SLOTS; ++slot) {
        // textures
        const std::string bufAccess = std::format("[{}]", slot);
        const int baseTextureBind = slot * MAX_MATERIAL_TEXTURE_COUNT;
        s_shader->setUniform((albedoU      + bufAccess), baseTextureBind + 0);
        s_shader->setUniform((emissiveU    + bufAccess), baseTextureBind + 1);
        s_shader->setUniform((reflectanceU + bufAccess), baseTextureBind + 2);
        s_shader->setUniform((normalU      + bufAccess), baseTextureBind + 3);
    }
}

void dr::PBRMaterial::bind(u32 slot) 
{
    DUST_PROFILE;

    m_boundSlot = slot;
    
    const std::string bufAccess = std::format("[{}]", slot);
    const std::string loc = "uMaterials" + bufAccess;
    s_shader->setUniform(loc + ".exist", true);

    // textures
    const int baseTextureBind = slot * MAX_MATERIAL_TEXTURE_COUNT;
    albedoTexture->bind(baseTextureBind + 0);
    emissivityTexture->bind(baseTextureBind + 1);
    reflectanceTexture->bind(baseTextureBind + 2);
    normalTexture->bind(baseTextureBind + 3);

    // colors
    s_shader->setUniform(loc + ".albedo", albedo);
    s_shader->setUniform(loc + ".ior", ior);
    s_shader->setUniform(loc + ".roughness", roughness);
}
void dr::PBRMaterial::unbind() 
{
    const std::string loc = shaderMaterialLoc(m_boundSlot);
    // unbind textures
    albedoTexture->unbind();
    reflectanceTexture->unbind();
    emissivityTexture->unbind();
    normalTexture->unbind();
    s_shader->setUniform(loc + ".exist", false);
}