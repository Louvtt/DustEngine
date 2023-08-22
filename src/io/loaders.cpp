#include "dust/io/loaders.hpp"
#include "dust/core/log.hpp"
#include "dust/io/assetsManager.hpp"
#include "dust/render/texture.hpp"
#include <thread>
namespace dr = dust::render;
namespace dio = dust::io;

#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#include <stb_image.h>

template <>
dust::Result<dr::Texture::Desc> 
dio::AssetsManager::LoadSync<dr::Texture::Desc, bool>(const dio::Path &_path, bool mipMaps) 
{
    auto path = fromAssetsDir(_path);
    if(!fs::exists(path)) {
        DUST_ERROR("[Texture] {} doesn't exists.", path.string());
        return {};
    }

    DUST_DEBUG("[Texture] Loading {}", path.string());
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    u8* data = stbi_load(path.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
    if(data == nullptr) {
        DUST_ERROR("[Texture][StbImage] Failed to load image : {}", stbi_failure_reason());
        return {};
    }

    auto texDesc = dr::Texture::Desc{
        data, (u32)width, (u32)height, (u32)nrChannels, dr::Texture::Filter::Linear, dr::Texture::Wrap::NoWrap, mipMaps
    };

    stbi_image_free(data);
    return texDesc;
}

//////////////////////////

#include "assimp/Importer.hpp"
#include "assimp/Vertex.h"
#include "assimp/color4.h"
#include "assimp/defs.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/types.h"

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
            auto texDesc = dio::AssetsManager::LoadSync<dr::Texture::Desc>(texPath, true);
            if(texDesc.has_value()) {
                diffuseTexture = new dr::Texture(texDesc.value());
            }
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

template <>
dust::Result<Ref<dr::Model>> 
dio::AssetsManager::LoadSync<Ref<dr::Model>>(const dio::Path &_path)
{
    auto path = fromAssetsDir(_path);
    if(!fs::exists(path)) {
        DUST_ERROR("[Model] {} doesn't exists.", path.string());
        return nullptr;
    }

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate               | 
        aiProcess_RemoveRedundantMaterials
    );

    fs::path modelPathDir = fs::path(path).parent_path();
    auto materials = processMaterials(scene, modelPathDir);
    auto processedMeshes = processMeshes(scene, materials);

    // importer.FreeScene();

    return dust::createRef<dr::Model>(processedMeshes);
}
//////////////////////////////

template<> void                                      \
dio::AssetsManager::LoadAsync<Ref<render::Model>>(const Path &path, ResultPtr<render::ModelPtr> result)
{   
    DUST_WARN("[AssetsManager] Async Model Loader isn't working yet.");
    return;
    // std::thread& loadThread = m_threadPool.at(m_usedThreads);
    // loadThread = std::thread([result, path]() {
    //     DUST_DEBUG("Loading {}...", path.string());
    //     const auto &res = dio::AssetsManager::LoadSync<Ref<render::Model>>(path);
    //     if(res.has_value()) {
    //         DUST_DEBUG("Loaded {}", path.string());
    //         *result = res.value();
    //     }
    //     AssetsManager::m_usedThreads--;
    // });
}

//////////////////////////

template <>
dust::Result<std::string> 
dio::AssetsManager::LoadSync<std::string>(const dio::Path &_path) 
{
    auto path = fromAssetsDir(_path);
    std::error_code error{};
    if(!fs::exists(path, error)) {
        DUST_ERROR("[File] {} doesn't exists (error {} : {})", path.string(), error.value(), error.message());
        return {};
    }

    std::ifstream in(path, std::ios::in);
    in.exceptions(std::ios::badbit | std::ios::failbit);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return contents;
    }
    return {};
}