#include "dust/render/material.hpp"

#include "dust/render/texture.hpp"
#include "dust/render/shader.hpp"

namespace dr = dust::render;

dr::Material::Material() {}


dr::TextureMaterial::TextureMaterial(dr::Texture *texture)
: m_texture(texture) {}

dr::TextureMaterial::~TextureMaterial()
{
    if(m_texture) delete m_texture;
}

void dr::TextureMaterial::bind(Shader *shader)
{
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

    shader->setUniform("uMaterial.texture", 0);
    shader->setUniform("uMaterial.hasTexture", false);
}

dr::PBRMaterial::PBRMaterial(Texture* diffuseTexture, glm::vec4 ambient, glm::vec4 diffuse)
: m_diffuse(diffuseTexture),
m_ambientColor(ambient),
m_diffuseColor(diffuse)
{ }

dr::PBRMaterial::~PBRMaterial()
{
    if(m_diffuse) delete m_diffuse;
}

void dr::PBRMaterial::bind(Shader *shader) 
{
    if(m_diffuse) m_diffuse->bind(0);
    shader->setUniform("uMaterial.diffuseTexture", 0);
    shader->setUniform("uMaterial.hasDiffuse",(m_diffuse != nullptr));
    // colors
    shader->setUniform("uMaterial.diffuse", m_diffuseColor);
    shader->setUniform("uMaterial.ambient", m_ambientColor);
}
void dr::PBRMaterial::unbind(Shader *shader) 
{
    if(m_diffuse) m_diffuse->unbind();
    shader->setUniform("uMaterial.hasDiffuse", false);

}