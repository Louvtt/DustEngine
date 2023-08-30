#include "dust/render/material.hpp"

#include "dust/render/texture.hpp"
#include "dust/render/shader.hpp"
#include "glm/ext/vector_float4.hpp"
#include <format>

namespace dr = dust::render;

dr::Material::Material()
: m_boundSlot(0) {}

static std::string shaderMaterialLoc(u32 index) {
    return std::format("uMaterial[{}]", index);
}

dr::ColorMaterial::ColorMaterial(glm::vec3 _color)
: color(_color) { }

void dr::ColorMaterial::bind(ShaderPtr shader, u32 slot)   
{
    m_boundSlot = slot;
    const std::string loc = shaderMaterialLoc(slot);
    shader->setUniform(loc + ".exist", true);
    shader->setUniform(loc + ".diffuse", glm::vec4{color, 1.f});
}
void dr::ColorMaterial::unbind(ShaderPtr shader) 
{
    const std::string loc = shaderMaterialLoc(m_boundSlot);
    shader->setUniform(loc + ".exist", false);
}

dr::TextureMaterial::TextureMaterial()
: texture(Texture::GetNullTexture()) {}
void dr::TextureMaterial::bind(ShaderPtr shader, u32 slot)
{
    m_boundSlot = slot;
    const std::string loc = shaderMaterialLoc(slot);
    shader->setUniform(loc + ".exist", true);
    texture->bind(0);
    shader->setUniform(loc + ".texture", 0);
}

void dr::TextureMaterial::unbind(ShaderPtr shader)
{
    const std::string loc = shaderMaterialLoc(m_boundSlot);
    texture->unbind();
    shader->setUniform("uHasMaterial", false);
    shader->setUniform("uMaterial.texture", 0);
}

dr::PBRMaterial::PBRMaterial()
: albedo({1.f, 1.f, 1.f}),
metallic(0.f),
roughness(0.f),
albedoTexture(Texture::GetNullTexture()),
metallicTexture(Texture::GetNullTexture()),
roughnessTexture(Texture::GetNullTexture())
{ }

void dr::PBRMaterial::bind(ShaderPtr shader, u32 slot) 
{
    m_boundSlot = slot;
    const std::string loc = shaderMaterialLoc(slot);
    // textures
    shader->setUniform(loc + ".exist", true);
    albedoTexture->bind(0);
    shader->setUniform(loc + ".albedoTex", 0);
    metallicTexture->bind(1);
    shader->setUniform(loc + ".metallicTex", 1);
    roughnessTexture->bind(2);
    shader->setUniform(loc + ".roughnessTex", 2);

    // colors
    shader->setUniform(loc + ".albedo", albedo);
    shader->setUniform(loc + ".metallic", metallic);
    shader->setUniform(loc + ".roughness", roughness);
}
void dr::PBRMaterial::unbind(ShaderPtr shader) 
{
    const std::string loc = shaderMaterialLoc(m_boundSlot);
    // unbind textures
    albedoTexture->unbind();
    metallicTexture->unbind();
    roughnessTexture->unbind();
    shader->setUniform(loc + ".exist", false);
}