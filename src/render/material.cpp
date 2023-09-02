#include "dust/render/material.hpp"

#include "dust/render/texture.hpp"
#include "dust/render/shader.hpp"
#include "dust/core/profiling.hpp"
#include "glm/ext/vector_float4.hpp"
#include <format>


namespace dr = dust::render;

dr::Material::Material()
: m_boundSlot(0) {}

static std::string shaderMaterialLoc(u32 index) {
    DUST_PROFILE;
    return std::format("uMaterials[{}]", index);
}

dr::ColorMaterial::ColorMaterial(glm::vec3 _color)
: color(_color) { }

void dr::ColorMaterial::bind(ShaderPtr shader, u32 slot)   
{
    DUST_PROFILE;
    m_boundSlot = slot;
    const std::string loc = shaderMaterialLoc(slot);
    shader->setUniform(loc + ".exist", true);
    shader->setUniform(loc + ".diffuse", glm::vec4{color, 1.f});
}
void dr::ColorMaterial::unbind(ShaderPtr shader) 
{
    DUST_PROFILE;
    const std::string loc = shaderMaterialLoc(m_boundSlot);
    shader->setUniform(loc + ".exist", false);
}

dr::TextureMaterial::TextureMaterial()
: texture(Texture::GetNullTexture()) {}
void dr::TextureMaterial::bind(ShaderPtr shader, u32 slot)
{
    DUST_PROFILE;
    m_boundSlot = slot;
    const std::string loc = shaderMaterialLoc(slot);
    shader->setUniform(loc + ".exist", true);
    texture->bind(0);
    shader->setUniform(loc + ".albedo", (int)(slot * MAX_MATERIAL_TEXTURE_COUNT));
}

void dr::TextureMaterial::unbind(ShaderPtr shader)
{
    DUST_PROFILE;
    const std::string loc = shaderMaterialLoc(m_boundSlot);
    texture->unbind();
    shader->setUniform("uHasMaterial", false);
    shader->setUniform("uMaterial.albedoTex", (int)(m_boundSlot * MAX_MATERIAL_TEXTURE_COUNT));
}

dr::PBRMaterial::PBRMaterial()
: albedo({1.f, 1.f, 1.f}),
reflectance(0.f),
roughness(0.f),
albedoTexture(Texture::GetNullTexture()),
reflectanceTexture(Texture::GetNullTexture()),
emissivityTexture(Texture::GetNullTexture()),
normalTexture(Texture::GetNullTexture())
{ }

void dr::PBRMaterial::bind(ShaderPtr shader, u32 slot) 
{
    DUST_PROFILE;
    m_boundSlot = slot;
    const std::string loc = shaderMaterialLoc(slot);
    const int baseTextureBind = slot * MAX_MATERIAL_TEXTURE_COUNT;
    // textures
    shader->setUniform(loc + ".exist", true);
    albedoTexture->bind(baseTextureBind + 0);
    shader->setUniform(loc + ".albedoTex", baseTextureBind + 0);
    emissivityTexture->bind(baseTextureBind + 1);
    shader->setUniform(loc + ".emissivityTex", baseTextureBind + 1);
    reflectanceTexture->bind(baseTextureBind + 2);
    shader->setUniform(loc + ".reflectanceTex", baseTextureBind + 2);
    normalTexture->bind(baseTextureBind + 3);
    shader->setUniform(loc + ".normalTex", baseTextureBind + 3);

    // colors
    shader->setUniform(loc + ".albedo", albedo);
    shader->setUniform(loc + ".reflectance", reflectance);
    shader->setUniform(loc + ".roughness", roughness);
}
void dr::PBRMaterial::unbind(ShaderPtr shader) 
{
    const std::string loc = shaderMaterialLoc(m_boundSlot);
    // unbind textures
    albedoTexture->unbind();
    reflectanceTexture->unbind();
    emissivityTexture->unbind();
    normalTexture->unbind();
    shader->setUniform(loc + ".exist", false);
}