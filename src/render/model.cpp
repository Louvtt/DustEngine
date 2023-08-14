#include "dust/render/model.hpp"

#include "dust/core/log.hpp"
#include "dust/render/mesh.hpp"
#include "dust/render/material.hpp"
#include "dust/render/texture.hpp"
#include <filesystem>
#include <unordered_map>

namespace dr = dust::render;

dr::Model::Model(const std::vector<dr::Mesh*> &meshes)
: m_meshes(meshes),
m_modelMat(1.f)
{ }
dr::Model::~Model()
{
    for(auto mesh : m_meshes) {
        delete mesh;
    }
    m_meshes.clear();
}


void dr::Model::draw(Shader *shader)
{
    shader->setUniform("uModel", m_modelMat);
    for (auto mesh : m_meshes) {
        if(!mesh) { continue; }
        mesh->draw(shader);
    }
}