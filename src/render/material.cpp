#include "dust/render/material.hpp"

#include "dust/render/texture.hpp"
#include "dust/render/shader.hpp"
#include "glm/ext/vector_float4.hpp"

namespace dr = dust::render;

dr::Material::Material() {}


dr::ColorMaterial::ColorMaterial(glm::vec3 color)
: m_color(color) { }

void dr::ColorMaterial::bind(Shader *shader)   
{
    shader->setUniform("uHasMaterial", true);
    shader->setUniform("uMaterial.hasDiffuse",false);
    shader->setUniform("uMaterial.diffuse", glm::vec4{m_color, 1.f});
    shader->setUniform("uMaterial.ambient", glm::vec4{m_color, 1.f});
}
void dr::ColorMaterial::unbind(Shader *shader) 
{
    shader->setUniform("uHasMaterial", false);
}

dr::TextureMaterial::TextureMaterial(dr::Texture *texture)
: m_texture(texture) {}

dr::TextureMaterial::~TextureMaterial()
{
    if(m_texture) delete m_texture;
}

void dr::TextureMaterial::bind(Shader *shader)
{
    shader->setUniform("uHasMaterial", true);
    if(m_texture) {
        m_texture->bind(0);
        shader->setUniform("uMaterial.texture", 0);
        shader->setUniform("uMaterial.hasTexture", true);
    } else {
        shader->setUniform("uMaterial.hasTexture", false);
    }
}

void dr::TextureMaterial::unbind(Shader *shader)
{
    if(m_texture) {
        m_texture->unbind();
    }
    shader->setUniform("uHasMaterial", false);
    shader->setUniform("uMaterial.texture", 0);
    shader->setUniform("uMaterial.hasTexture", false);
}

dr::PBRMaterial::PBRMaterial(const dr::PBRMaterial::Data& data)
: m_data(data)
{ }

dr::PBRMaterial::~PBRMaterial()
{
    if(m_data.diffuse) delete m_data.diffuse;
}

void dr::PBRMaterial::bind(Shader *shader) 
{
    // textures
    shader->setUniform("uHasMaterial", true);
    if(m_data.diffuse) m_data.diffuse->bind(0);
    shader->setUniform("uMaterial.diffuseTexture", 0);
    shader->setUniform("uMaterial.hasDiffuse",(m_data.diffuse != nullptr));

    // colors
    shader->setUniform("uMaterial.diffuse", m_data.diffuseColor);
    shader->setUniform("uMaterial.ambient", m_data.ambientColor);
    shader->setUniform("uMaterial.specular", m_data.specularColor);
    shader->setUniform("uMaterial.shininess", m_data.shininess);
}
void dr::PBRMaterial::unbind(Shader *shader) 
{
    // unbind textures
    if(m_data.diffuse) m_data.diffuse->unbind();
    shader->setUniform("uHasMaterial", false);
}