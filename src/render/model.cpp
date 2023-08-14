#include "dust/render/model.hpp"

#include "assimp/Importer.hpp"
#include "assimp/Vertex.h"
#include "assimp/color4.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/types.h"
#include "dust/core/log.hpp"
#include "dust/render/mesh.hpp"
#include "dust/render/material.hpp"
#include "dust/render/texture.hpp"
#include <filesystem>

namespace dr = dust::render;

dr::Model::Model(std::vector<dr::Mesh*> mesh)
: m_meshes(mesh),
m_modelMat(1.f)
{ }
dr::Model::~Model()
{
    for(auto mesh : m_meshes) {
        delete mesh;
    }
    m_meshes.clear();
}

static std::vector<dr::Material*> 
processMaterials(const aiScene *scene, const std::filesystem::path& basePath)
{
    std::vector<dr::Material*> materials{};
    materials.reserve(scene->mNumMaterials);
    for(int i = 0; i < scene->mNumMaterials; ++i) {
        auto material = scene->mMaterials[i];
        DUST_DEBUG("[Model] Material {}", i);
        aiString filePath;
        dr::Texture* diffuseTexture = nullptr;
        if(material->GetTexture(aiTextureType_DIFFUSE, 0, &filePath) == AI_SUCCESS) {
            const std::string texPath = basePath.string() + '/' + filePath.C_Str();
            DUST_DEBUG("Texture found for mat at {}", texPath);
            diffuseTexture = new dust::render::Texture(texPath);
        }

        glm::vec4 diffuse(1.f);
        aiColor4D diffuseMat;
        if(aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseMat) == AI_SUCCESS) {
            diffuse = {diffuseMat.r, diffuseMat.g, diffuseMat.b, diffuseMat.a};
        }

        glm::vec4 ambient(1.f);
        aiColor4D ambientMat;
        if(aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambientMat) == AI_SUCCESS) {
            ambient = {ambientMat.r, ambientMat.g, ambientMat.b, ambientMat.a};
        }

        materials.push_back(new dust::render::PBRMaterial(
            diffuseTexture, ambient, diffuse
        ));
    }
    return materials;
}

static dr::Mesh* processMesh(aiMesh *mesh, const std::vector<dr::Material*> &materials)
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

    auto res = new dr::Mesh(&vertices.front(), sizeof(dr::Model::Vertex), mesh->mNumVertices, indices, attributes);
    res->setMaterial(materials.at(mesh->mMaterialIndex));
    return res;
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

    std::filesystem::path modelPathDir = std::filesystem::path(path).parent_path();
    auto materials = processMaterials(scene, modelPathDir);

    std::vector<dr::Mesh*> processedMeshes{};
    for (int i = 0; i < scene->mNumMeshes; ++i) {
        processedMeshes.push_back(processMesh(scene->mMeshes[i], materials));
    }

    importer.FreeScene();

    return new Model(processedMeshes);
}


void dr::Model::draw(Shader *shader)
{
    shader->setUniform("uModel", m_modelMat);
    for (auto mesh : m_meshes) {
        mesh->draw(shader);
    }
}