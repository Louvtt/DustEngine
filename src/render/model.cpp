#include "dust/render/model.hpp"

#include "dust/core/log.hpp"
#include "dust/render/mesh.hpp"
#include "dust/render/material.hpp"
#include "dust/render/texture.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <filesystem>
#include <unordered_map>

namespace dr = dust::render;

dr::Model::Model(MeshPtr mesh)
: m_meshes({mesh}),
m_position(.0f, .0f, .0f),
m_modelMat(1.f) {}

dr::Model::Model(const std::vector<dr::MeshPtr> &meshes)
: m_meshes(meshes),
m_position(.0f, .0f, .0f),
m_modelMat(1.f)
{ }
dr::Model::~Model()
{
    for(auto mesh : m_meshes) {
        mesh.reset();
    }
    m_meshes.clear();
}

void dr::Model::setPosition(glm::vec3 position)
{
    m_position = position;
    m_modelMat = glm::translate(glm::mat4(1.f), position); 
}

glm::vec3 dr::Model::getPosition() const
{
    return m_position;
}


void dr::Model::draw(ShaderPtr shader)
{
    shader->setUniform("uModel", m_modelMat);
    for (auto mesh : m_meshes) {
        if(!mesh) { continue; }
        mesh->draw(shader);
    }
}