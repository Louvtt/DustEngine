#include "dust/io/loaders.hpp"
#include "dust/core/log.hpp"
#include "dust/core/types.hpp"
#include "dust/io/assetsManager.hpp"
#include "dust/render/material.hpp"
#include "dust/render/mesh.hpp"
#include "dust/render/texture.hpp"
#include <algorithm>
#include <thread>
namespace dr = dust::render;
namespace dio = dust::io;

#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#include <stb_image.h>
#include <nv_dds.h>

dust::Result<dr::TexturePtr> 
dio::LoadTexture2D(const dio::Path &_path) 
{
    auto path = AssetsManager::FromAssetsDir(_path);
    if(!fs::exists(path)) {
        DUST_ERROR("[Texture] {} doesn't exists.", path.string());
        return {};
    }
    DUST_DEBUG("[Texture] Loading {}", path.string());

    // NVIDIA DDS
    if(path.extension() == ".dds") {
        nv_dds::CDDSImage image{};
        image.load(path.string());
        if(image.get_type() == nv_dds::TextureType::TextureFlat) {
            render::TexturePtr res = render::Texture::CreateTextureRaw(
                GL_TEXTURE_2D,
                image.get_width(), 
                image.get_height(), 
                image.get_components()
            );
            res->bind();
            image.upload_texture2D();
            res->unbind();
            image.clear();
            return res;
        } else if(image.get_type() == nv_dds::TextureType::TextureCubemap) {
            render::TexturePtr res = render::Texture::CreateTextureRaw(
                GL_TEXTURE_CUBE_MAP,
                image.get_width(), 
                image.get_height(), 
                image.get_components()
            );
            res->bind();
            image.upload_textureCubemap();
            res->unbind();
            image.clear();
            return res;
        }
    }
    // STB_IMAGE 
    else { 
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        u8* data = stbi_load(path.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
        if(data == nullptr) {
            DUST_ERROR("[Texture][StbImage] Failed to load image : {}", stbi_failure_reason());
            return {};
        }
        
        render::TexturePtr res = render::Texture::CreateTexture2D(
            width, 
            height, 
            nrChannels,
            data,
            {
                dr::TextureFilter::Linear, 
                dr::TextureWrap::NoWrap, 
                true
            }
        );

        stbi_image_free(data);
        return res;
    }
    return {};
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

static std::vector<dr::MaterialPtr> 
processMaterials(const aiScene *scene, const std::filesystem::path& basePath)
{
    std::vector<dr::MaterialPtr> materials{};
    materials.reserve(scene->mNumMaterials);
    for(int i = 0; i < scene->mNumMaterials; ++i) {
        const auto material = scene->mMaterials[i];
        Ref<render::PBRMaterial> mat = createRef<render::PBRMaterial>();


        aiString filePath;
        ai_real factor;
        aiColor4D color;

        // ALBEDO
        if(material->GetTexture(aiTextureType_DIFFUSE, 0, &filePath) == AI_SUCCESS) {
            const dio::Path texPath = basePath / dio::Path(filePath.C_Str());
            const auto texture = dio::LoadTexture2D(texPath);
            if(texture.has_value()) {
                mat->albedoTexture = texture.value();
            }
        }
        if(aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
            mat->albedo = {color.r, color.g, color.b};
        }

        // Metallic
        if(material->GetTexture(AI_MATKEY_METALLIC_TEXTURE, &filePath) == AI_SUCCESS) {
            const dio::Path texPath = basePath / dio::Path(filePath.C_Str());
             const auto texture = dio::LoadTexture2D(texPath);
            if(texture.has_value()) {
                mat->metallicTexture = texture.value();
            }
        }
        if(aiGetMaterialFloat(material, AI_MATKEY_METALLIC_FACTOR, &factor) == aiReturn_SUCCESS) {
            mat->metallic = factor;
        }

        // Roughness
        if(material->GetTexture(AI_MATKEY_ROUGHNESS_TEXTURE, &filePath) == AI_SUCCESS) {
            const dio::Path texPath = basePath / dio::Path(filePath.C_Str());
            const auto texture = dio::LoadTexture2D(texPath);
            if(texture.has_value()) {
                mat->metallicTexture = texture.value();
            }
        }
        if(aiGetMaterialFloat(material, AI_MATKEY_ROUGHNESS_FACTOR, &factor) == aiReturn_SUCCESS) {
            mat->metallic = factor;
        }

        // Clearcoat
        // Emissive
        // Normals
        // Transmission
        // ...

        materials.push_back(mat);
    }
    return materials;
}

static std::vector<dr::MeshPtr> 
processMeshes(const aiScene *scene, std::vector<dr::MaterialPtr> materials)
{   
    // Attributes
    std::vector<dr::Attribute> attributes {
        dr::Attribute::Pos3D,
        dr::Attribute::Pos3D,     // normals
        dr::Attribute::TexCoords,
        dr::Attribute::Color,
        dr::Attribute::Float      // matID
    };

    // batch all of the model into as less draw calls as possible
    const u32 batchCount = std::ceil((float)materials.size() / (float)DUST_MATERIAL_SLOTS); 
    // pre calculate the number of vertices
    std::vector<u32> numTotalVertices(batchCount, 0u);
    std::vector<u32> numTotalIndices(batchCount, 0u);
    for(int i = 0; i < scene->mNumMeshes; ++i) {
        const u32 matId = scene->mMeshes[i]->mMaterialIndex;
        const u32 batchIdx = std::floor(matId / (float)DUST_MATERIAL_SLOTS);
        numTotalVertices[batchIdx] += scene->mMeshes[i]->mNumVertices;
        numTotalIndices[batchIdx]  += scene->mMeshes[i]->mNumFaces * 3;
    }
    // allocate space for vertices
    std::unordered_map<int, std::vector<dr::ModelVertex>> vertices(batchCount);
    std::unordered_map<int,std::vector<u32>> indices(batchCount);
    for(int i = 0; i < batchCount; ++i) {
        vertices.insert({i, std::vector<dr::ModelVertex>(numTotalVertices[i])});
        indices.insert({i, std::vector<u32>(numTotalIndices[i])});
    }

    // parse all meshes
    for (int i = 0; i < scene->mNumMeshes; ++i) {
        auto mesh = scene->mMeshes[i];
        const u32 matId = scene->mMeshes[i]->mMaterialIndex;
        const u32 batchIdx = std::floor((float)matId / (float)DUST_MATERIAL_SLOTS);
        const u32 previousVertexCount = vertices[batchIdx].size();

        // process vertices
        for(int i = 0; i < mesh->mNumVertices; ++i) {
            auto pos    = mesh->mVertices[i];
            auto color  = mesh->mColors[i];

            dr::ModelVertex vertex = {{ pos.x, pos.y, pos.z }};
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
            vertex.materialID = (float)(matId % DUST_MATERIAL_SLOTS); // matId in batch 

            vertices[batchIdx].push_back(vertex);
        }
        // parses mesh indices and offset it by the previous number of vertices
        for(int i = 0; i < mesh->mNumFaces; ++i) {
            auto face = mesh->mFaces[i];
            // only manage triangles
            if(face.mNumIndices != 3) continue;
            indices[batchIdx].push_back(previousVertexCount + face.mIndices[0]);
            indices[batchIdx].push_back(previousVertexCount + face.mIndices[1]);
            indices[batchIdx].push_back(previousVertexCount + face.mIndices[2]);
        }
    }

    // create meshes
    u32 materialI = 0;
    std::vector<dr::MeshPtr> res(batchCount);
    for(int i = 0; i < batchCount; ++i) {
        auto mesh = createRef<render::Mesh>(
            &vertices[i].front(),
            sizeof(dr::ModelVertex),
            vertices[i].size(),
            indices[i],
            attributes
        );
        for(int m = 0; m < DUST_MATERIAL_SLOTS; ++m) {
            if(materialI >= materials.size()) break;
            mesh->setMaterial(m, materials.at(materialI));
            materialI += 1;
        }
        res.push_back(mesh);
    }
    return res;
}

dust::Result<dr::ModelPtr> 
dio::LoadModel(const dio::Path &_path)
{
    auto path = AssetsManager::FromAssetsDir(_path);
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

void                                      \
dio::LoadModelAsync(const Path &path, ResultPtr<render::ModelPtr> result)
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

dust::Result<std::string> 
dio::LoadFile(const dio::Path &_path) 
{
    auto path = AssetsManager::FromAssetsDir(_path);
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