#include "dust/render/model.hpp"

#include "assimp/Importer.hpp"
#include "assimp/Vertex.h"
#include "assimp/color4.h"
#include "assimp/defs.h"
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

static std::vector<dr::Material*> 
processMaterials(const aiScene *scene, const std::filesystem::path& basePath)
{
    std::vector<dr::Material*> materials{};
    materials.reserve(scene->mNumMaterials);
    for(int i = 0; i < scene->mNumMaterials; ++i) {
        auto material = scene->mMaterials[i];
        // DUST_DEBUG("[Model] Material {}", i);
        aiString filePath;
        dr::Texture* diffuseTexture = nullptr;
        if(material->GetTexture(aiTextureType_DIFFUSE, 0, &filePath) == AI_SUCCESS) {
            const std::string texPath = basePath.string() + '/' + filePath.C_Str();
            // DUST_DEBUG("Texture found for mat at {}", texPath);
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

        glm::vec4 specular(1.f);
        aiColor4D specularMat;
        if(aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specularMat) == AI_SUCCESS) {
            specular = {specularMat.r, specularMat.g, specularMat.b, specularMat.a};
        }

        float shininess = 0.f;
        ai_real shininessMat;
        if(aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininessMat) == AI_SUCCESS) {
            shininess = (float)shininessMat;
        }

        materials.push_back(new dr::PBRMaterial({
            diffuseTexture, 
            ambient, diffuse, specular, shininess
        }));
    }
    return materials;
}

static std::vector<dr::Mesh*> processMeshes(const aiScene *scene, const std::vector<dr::Material*> &materials)
{   
    // Attributes
    std::vector<dr::Attribute> attributes {
        dr::Attribute::Pos3D,
        dr::Attribute::TexCoords,
        dr::Attribute::Pos3D,
        dr::Attribute::Color
    };

    // batch all of the model into one draw call
    const u32 matCount = materials.size(); 
    // pre calculate the number of vertices
    std::vector<u32> numTotalVertices(matCount, 0);
    std::vector<u32> numTotalIndices(matCount, 0);
    for(int i = 0; i < scene->mNumMeshes; ++i) {
        u32 matIndex = scene->mMeshes[i]->mMaterialIndex;
        numTotalVertices[matIndex] += scene->mMeshes[i]->mNumVertices;
        numTotalIndices[matIndex]  += scene->mMeshes[i]->mNumFaces * 3;
    }
    // allocate space for vertices
    std::unordered_map<int, std::vector<dr::Model::Vertex>> vertices(matCount);
    std::unordered_map<int,std::vector<u32>> indices(matCount);
    for(int i = 0; i < matCount; ++i) {
        vertices.insert({i, std::vector<dr::Model::Vertex>(numTotalVertices[i])});
        indices.insert({i, std::vector<u32>(numTotalIndices[i])});
    }

    // parse all meshes
    for (int i = 0; i < scene->mNumMeshes; ++i) {
        auto mesh = scene->mMeshes[i];
        const u32 matIndex = scene->mMeshes[i]->mMaterialIndex;
        const u32 previousVertexCount = vertices[matIndex].size();

        // process vertices
        for(int i = 0; i < mesh->mNumVertices; ++i) {
            auto pos    = mesh->mVertices[i];
            auto color  = mesh->mColors[i];

            dr::Model::Vertex vertex = {{ pos.x, pos.y, pos.z }};
            if(mesh->HasTextureCoords(0)) { 
                auto tex   = mesh->mTextureCoords[0][i];
                vertex.tex = { tex.x, tex.y }; 
            }
            if(mesh->HasNormals()) { 
                auto normal = mesh->mNormals[i];
                vertex.normal = { normal.x, normal.y, normal.z};
            }
            if(mesh->HasVertexColors(i)) {
                auto color = mesh->mColors[i];
                vertex.color = { color->r, color->g, color->b, color->a };
            }
            vertices[matIndex].push_back(vertex);
        }
        // parses mesh indices and offset it by the previous number of vertices
        for(int i = 0; i < mesh->mNumFaces; ++i) {
            auto face = mesh->mFaces[i];
            // only manage triangles
            if(face.mNumIndices != 3) continue;
            indices[matIndex].push_back(previousVertexCount + face.mIndices[0]);
            indices[matIndex].push_back(previousVertexCount + face.mIndices[1]);
            indices[matIndex].push_back(previousVertexCount + face.mIndices[2]);
        }
    }

    // create meshes
    std::vector<dr::Mesh*> res(matCount);
    for(int i = 0; i < matCount; ++i) {
        auto mesh = new dr::Mesh(
            &vertices[i].front(),
            sizeof(dr::Model::Vertex),
            vertices[i].size(),
            indices[i],
            attributes
        );
        mesh->setMaterial(materials.at(i));
        res.push_back(mesh);
    }
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
        aiProcess_Triangulate               | 
        aiProcess_RemoveRedundantMaterials
    );

    std::filesystem::path modelPathDir = std::filesystem::path(path).parent_path();
    auto materials = processMaterials(scene, modelPathDir);
    auto processedMeshes = processMeshes(scene, materials);

    // importer.FreeScene();

    return new Model(processedMeshes);
}


void dr::Model::draw(Shader *shader)
{
    shader->setUniform("uModel", m_modelMat);
    for (auto mesh : m_meshes) {
        if(!mesh) { continue; }
        mesh->draw(shader);
    }
}