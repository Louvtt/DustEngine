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
roughness(0.f),
metallic(0.f),
ao(0.0),
albedoTexture(Texture::GetNullTexture()),
normalTexture(Texture::GetNullTexture()),
metallicTexture(Texture::GetNullTexture()),
roughnessTexture(Texture::GetNullTexture()),
aoTexture(Texture::GetNullTexture())
{  }

void dr::PBRMaterial::SetupMaterialShader(Shader *shader)
{
    DUST_PROFILE;
    s_shader = shader;
    static constexpr const char* albedoU      = "uMaterials[{}].texAlbedo";
    static constexpr const char* normalU      = "uMaterials[{}].texNormal";
    static constexpr const char* metallicU    = "uMaterials[{}].texMetallic";
    static constexpr const char* roughnessU   = "uMaterials[{}].texRoughness";
    static constexpr const char* aoU          = "uMaterials[{}].texAO";
    for(int slot = 0; slot < DUST_MATERIAL_SLOTS; ++slot) {
        // textures
        const int baseTextureBind = slot * MAX_MATERIAL_TEXTURE_COUNT;
        s_shader->setUniform(std::format(albedoU     , slot), baseTextureBind + 0);
        s_shader->setUniform(std::format(normalU     , slot), baseTextureBind + 1);
        s_shader->setUniform(std::format(metallicU   , slot), baseTextureBind + 2);
        s_shader->setUniform(std::format(roughnessU  , slot), baseTextureBind + 3);
        s_shader->setUniform(std::format(aoU         , slot), baseTextureBind + 4);
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
    normalTexture->bind(baseTextureBind + 1);
    metallicTexture->bind(baseTextureBind + 2);
    roughnessTexture->bind(baseTextureBind + 3);
    aoTexture->bind(baseTextureBind + 4);

    // colors
    s_shader->setUniform(loc + ".albedo", albedo);
    s_shader->setUniform(loc + ".metallic", metallic);
    s_shader->setUniform(loc + ".roughness", roughness);
    s_shader->setUniform(loc + ".ao", ao);
}
void dr::PBRMaterial::unbind() 
{
    const std::string loc = shaderMaterialLoc(m_boundSlot);
    // unbind textures
    albedoTexture->unbind();
    normalTexture->unbind();
    metallicTexture->unbind();
    roughnessTexture->unbind();
    aoTexture->unbind();
    s_shader->setUniform(loc + ".exist", false);
}