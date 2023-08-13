#include "dust/render/model.hpp"

#include "assimp/Importer.hpp"
#include "assimp/Vertex.h"
#include "assimp/mesh.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "dust/core/log.hpp"
#include "dust/render/mesh.hpp"
#include <filesystem>

namespace dr = dust::render;

dr::Model::Model(std::vector<dr::Mesh*> mesh)
: m_meshes(mesh)
{ }
dr::Model::~Model()
{
    m_meshes.clear();
}

static dr::Mesh* processMesh(aiMesh *mesh)
{
    std::vector<dr::Model::Vertex> vertices;
    vertices.reserve(mesh->mNumVertices);

    // Attributes
    std::vector<dr::Attribute> attributes {
        dr::Attribute::Pos3D,
        dr::Attribute::TexCoords,
        dr::Attribute::Pos3D,
        dr::Attribute::Color
    };

    // process vertices
    for(int i = 0; i < mesh->mNumVertices; ++i) {
        auto pos    = mesh->mVertices[i];
        auto color  = mesh->mColors[i];

        dr::Model::Vertex vertex = {{ pos.x, pos.y, pos.z }};
        if(mesh->HasTextureCoords(i)) { 
            auto tex   = mesh->mTextureCoords[i];
            vertex.tex = { tex->x, tex->y }; 
        }
        if(mesh->HasNormals()) { 
            auto normal = mesh->mNormals[i];
            vertex.normal = { normal.x, normal.y, normal.z};
        }
        if(mesh->HasVertexColors(i)) {
            auto color = mesh->mColors[i];
            vertex.color = { color->r, color->g, color->b, color->a };
        }
        vertices.push_back(vertex);
    }

    // process indices
    std::vector<u32> indices{};
    indices.reserve(mesh->mNumFaces * 3);
    for(int i = 0; i < mesh->mNumFaces; ++i) {
        auto face = mesh->mFaces[i];
        // only manage triangles
        if(face.mNumIndices != 3) continue;

        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }

    return new dr::Mesh(&vertices.front(), sizeof(dr::Model::Vertex), mesh->mNumVertices, indices, attributes);
}

dr::Model* dr::Model::loadFromFile(const std::string& path)
{
    if(!std::filesystem::exists(path)) {
        DUST_ERROR("[Model] {} doesn't exists.", path);
        return nullptr;
    }

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate
    );

    std::vector<dr::Mesh*> processedMeshes{};
    for (int i = 0; i < scene->mNumMeshes; ++i) {
        processedMeshes.push_back(processMesh(scene->mMeshes[i]));
    }
    return new Model(processedMeshes);
}


void dr::Model::draw(Shader *shader)
{
    shader->use();
    for (auto mesh : m_meshes) {
        mesh->draw();
    }
}