#include "dust/io/loaders.hpp"
#include "dust/core/log.hpp"
#include "dust/core/profiling.hpp"
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

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#ifdef LOADER_NVDDS
#   include <nv_dds.h>
#endif

dust::Result<dr::TexturePtr> 
dio::_load_texture_general(const dio::Path &_path)
{
    DUST_PROFILE_SECTION("io::LoadTexture2D");
    auto path = AssetsManager::FromAssetsDir(_path);
    if(!fs::exists(path)) {
        DUST_ERROR("[Texture] {} doesn't exists.", path.string());
        return {};
    }
    DUST_DEBUG("[Texture] Loading {}", path.string());

    // NVIDIA DDS
#ifdef LOADER_NVDDS
    if(path.extension() == ".dds") {
        DUST_PROFILE_SECTION("io::LoadTexture2D nv_dds");
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
    else
#endif //LOADER_NVDDS
    // STB_IMAGE
    {
        DUST_PROFILE_SECTION("io::LoadTexture2D stb_image");
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        u8* data = stbi_load(path.string().c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
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

DUST_DEFINE_LOADER(dr::TexturePtr, Texture2D);

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
    DUST_PROFILE_SECTION("io::LoadModel processMaterials");
    std::vector<dr::MaterialPtr> materials{};
    materials.reserve(scene->mNumMaterials);
    for(int i = 0; i < scene->mNumMaterials; ++i) {
        const auto material = scene->mMaterials[i];
        Ref<render::PBRMaterial> mat = createRef<render::PBRMaterial>();

        aiString filePath;
        ai_real factor;
        aiColor4D color;

        // albedo
        if(material->GetTexture(aiTextureType_DIFFUSE, 0, &filePath) == AI_SUCCESS) {
            const dio::Path texPath = basePath / dio::Path(filePath.C_Str());
            const auto texture = dio::LoadTexture2D(texPath);
            if(texture.has_value()) {
                mat->albedoTexture = texture.value();
            }
        }
        if(material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
            mat->albedo = {color.r, color.g, color.b};
        }

        // Normals
        if(material->GetTexture(aiTextureType_HEIGHT, 0, &filePath) == AI_SUCCESS) {
            const dio::Path texPath = basePath / dio::Path(filePath.C_Str());
            const auto texture = dio::LoadTexture2D(texPath);
            if(texture.has_value()) {
                mat->normalTexture = texture.value();
            }
        }

        // Roughness
        if(material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &filePath) == AI_SUCCESS) {
            const dio::Path texPath = basePath / dio::Path(filePath.C_Str());
            const auto texture = dio::LoadTexture2D(texPath);
            if(texture.has_value()) {
                mat->roughnessTexture = texture.value();
            }
        }
        if(material->Get(AI_MATKEY_ROUGHNESS_FACTOR, factor) == aiReturn_SUCCESS) {
            mat->roughness = factor;
        }

        // Metallic
        if(material->GetTexture(aiTextureType_METALNESS, 0, &filePath) == AI_SUCCESS) {
            const dio::Path texPath = basePath / dio::Path(filePath.C_Str());
            const auto texture = dio::LoadTexture2D(texPath);
            if(texture.has_value()) {
                mat->metallicTexture = texture.value();
            }
        }
        if(material->Get(AI_MATKEY_METALLIC_FACTOR, factor) == aiReturn_SUCCESS) {
            mat->metallic = factor;
        }

        // AO
        if(material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &filePath) == AI_SUCCESS) {
            const dio::Path texPath = basePath / dio::Path(filePath.C_Str());
            const auto texture = dio::LoadTexture2D(texPath);
            if(texture.has_value()) {
                mat->aoTexture = texture.value();
            }
        }
        mat->ao = 1.f;

        // Name
        if(material->Get(AI_MATKEY_NAME, filePath) == AI_SUCCESS) {
            mat->setName(filePath.C_Str());
        }

        materials.push_back(mat);
    }
    return materials;
}

static std::vector<dr::MeshPtr> 
processMeshes(const aiScene *scene, std::vector<dr::MaterialPtr> materials)
{   
    DUST_PROFILE_SECTION("io::LoadModel processMeshes");
    // Attributes
    std::vector<dr::Attribute> attributes {
        dr::Attribute::Pos3D,
        dr::Attribute::TexCoords,
        dr::Attribute::Pos3D,     // normals
        dr::Attribute::Pos3D,     // tangents
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
    std::vector<std::vector<dr::ModelVertex>> vertices(batchCount);
    std::vector<std::vector<u32>> indices(batchCount);
    std::vector<std::string> batchMeshesName(batchCount, "");
    for(int i = 0; i < batchCount; ++i) {
        vertices.push_back(std::vector<dr::ModelVertex>(numTotalVertices[i]));
        indices.push_back(std::vector<u32>(numTotalIndices[i]));
    }

    // parse all meshes
    {
        DUST_PROFILE_SECTION("io::LoadModel parse meshes");
        for (int i = 0; i < scene->mNumMeshes; ++i) {
            auto mesh = scene->mMeshes[i];
            const u32 matId = scene->mMeshes[i]->mMaterialIndex;
            const u32 batchIdx = std::floor((float)matId / (float)DUST_MATERIAL_SLOTS);
            const u32 previousVertexCount = vertices[batchIdx].size();
            batchMeshesName.at(batchIdx) += std::string(mesh->mName.C_Str()) + " ";
            // process vertices
            for(int i = 0; i < mesh->mNumVertices; ++i) {
                const auto pos    = mesh->mVertices[i];

                dr::ModelVertex vertex = {{ pos.x, pos.y, pos.z }};
                if(mesh->HasTextureCoords(0)) { 
                    const auto tex   = mesh->mTextureCoords[0][i];
                    vertex.tex = { tex.x, tex.y }; 
                }
                if(mesh->HasNormals()) { 
                    const auto normal = mesh->mNormals[i];
                    vertex.normal = { normal.x, normal.y, normal.z};
                }
                if(mesh->HasTangentsAndBitangents()) {
                    const auto tangent = mesh->mTangents[i];
                    vertex.tangent = { tangent.x, tangent.y, tangent.z };

                    // orthonormalize tangent
                    vertex.tangent = glm::normalize(vertex.tangent - glm::dot(vertex.normal, vertex.tangent) * vertex.normal);
                    const auto bitangent_ = mesh->mBitangents[i];
                    const auto bitangent  = glm::vec3{bitangent_.x, bitangent_.y, bitangent_.z};
                    // for right-handed tbn
                    if(glm::dot(glm::cross(vertex.normal, vertex.tangent), bitangent) < 0.f)
                        vertex.tangent *= -1;
                }

                if(mesh->HasVertexColors(i)) {
                    const auto color = mesh->mColors[0][i];
                    vertex.color = { color.r, color.g, color.b, color.a };
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
    }

    // create meshes
    u32 materialI = 0;
    std::vector<dr::MeshPtr> res(batchCount);
    {
        DUST_PROFILE_SECTION("io::LoadModel create meshes");
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
            mesh->setName(batchMeshesName.at(i));
            res[i] = mesh;
        }
    }
    return res;
}


static std::vector<dr::MeshPtr>
processMeshesNoBatch(const aiScene *scene, std::vector<dr::MaterialPtr> materials)
{
    DUST_PROFILE_SECTION("io::LoadModel processMeshes");
    // Attributes
    std::vector<dr::Attribute> attributes {
        dr::Attribute::Pos3D,
        dr::Attribute::TexCoords,
        dr::Attribute::Pos3D,     // normals
        dr::Attribute::Pos3D,     // tangents
        dr::Attribute::Color,
        dr::Attribute::Float      // matID
    };

    std::vector<dr::MeshPtr> results{};
    results.reserve(scene->mNumMeshes);
    // parse all meshes
    {
        DUST_PROFILE_SECTION("io::LoadModel parse meshes");
        for (int mesh_i = 0; mesh_i < scene->mNumMeshes; ++mesh_i) {
            auto mesh = scene->mMeshes[mesh_i];
            std::vector<dr::ModelVertex> vertices;
            std::vector<unsigned int> indices;
            vertices.reserve(mesh->mNumVertices);
            indices.reserve(mesh->mNumVertices * 3);
            // process vertices
            for(int i = 0; i < mesh->mNumVertices; ++i) {
                const auto pos    = mesh->mVertices[i];

                dr::ModelVertex vertex = {{ pos.x, pos.y, pos.z }};
                if(mesh->HasTextureCoords(0)) {
                    const auto tex   = mesh->mTextureCoords[0][i];
                    vertex.tex = { tex.x, tex.y };
                }
                if(mesh->HasNormals()) {
                    const auto normal = mesh->mNormals[i];
                    vertex.normal = { normal.x, normal.y, normal.z};
                }
                if(mesh->HasTangentsAndBitangents()) {
                    const auto tangent = mesh->mTangents[i];
                    vertex.tangent = { tangent.x, tangent.y, tangent.z };

                    // orthonormalize tangent
                    vertex.tangent = glm::normalize(vertex.tangent - glm::dot(vertex.normal, vertex.tangent) * vertex.normal);
                    const auto bitangent_ = mesh->mBitangents[i];
                    const auto bitangent  = glm::vec3{bitangent_.x, bitangent_.y, bitangent_.z};
                    // for right-handed tbn
                    if(glm::dot(glm::cross(vertex.normal, vertex.tangent), bitangent) < 0.f)
                        vertex.tangent *= -1;
                }

                if(mesh->HasVertexColors(i)) {
                    const auto color = mesh->mColors[0][i];
                    vertex.color = { color.r, color.g, color.b, color.a };
                }
                vertex.materialID = 0;

                vertices.push_back(vertex);
            }
            // parses mesh indices and offset it by the previous number of vertices
            for(int i = 0; i < mesh->mNumFaces; ++i) {
                auto face = mesh->mFaces[i];
                // only manage triangles
                if(face.mNumIndices != 3) continue;
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
            }

            auto created_mesh = createRef<render::Mesh>(
                &vertices.front(),
                sizeof(dr::ModelVertex),
                vertices.size(),
                indices,
                attributes
            );
            created_mesh->setMaterial(0, materials.at(mesh_i));
            created_mesh->setName(std::string(mesh->mName.C_Str()));
            results.push_back(created_mesh);
        }
    }
    return results;
}

dust::Result<dr::ModelPtr>
dio::_load_model_gltf(const dust::io::Path &path) {
    Assimp::Importer importer {};
    DUST_INFO("Loading GLTF model {}...", path.filename().string());
    const aiScene* scene = importer.ReadFile(path.string(),
                                             aiProcess_Triangulate               |
                                                 aiProcess_RemoveRedundantMaterials  |
                                                 aiProcess_GenNormals                |
                                                 aiProcess_CalcTangentSpace          |
                                                 aiProcess_OptimizeMeshes            |
                                                 aiProcess_OptimizeGraph
    );
    if (scene == nullptr) return {};

    fs::path modelPathDir = fs::path(path).parent_path();
    DUST_INFO("Importing GLTF model {}...", path.filename().string());
    auto materials = processMaterials(scene, modelPathDir);
    auto processedMeshes = processMeshesNoBatch(scene, materials);
    return dust::createRef<dr::Model>(processedMeshes);
}

dust::Result<dr::ModelPtr>
dio::_load_model_general(const dust::io::Path &path) {
    Assimp::Importer importer {};
    DUST_INFO("Loading model {}...", path.filename().string());
    const aiScene* scene = importer.ReadFile(path.string(),
                                             aiProcess_Triangulate               |
                                                 aiProcess_RemoveRedundantMaterials  |
                                                 aiProcess_GenNormals                |
                                                 aiProcess_CalcTangentSpace          |
                                                 aiProcess_OptimizeMeshes            |
                                                 aiProcess_OptimizeGraph
    );
    if (scene == nullptr) return {};

    fs::path modelPathDir = fs::path(path).parent_path();

    DUST_INFO("Importing model {}...", path.filename().string());
    auto materials = processMaterials(scene, modelPathDir);
    auto processedMeshes = processMeshes(scene, materials);
    return dust::createRef<dr::Model>(processedMeshes);
}

DUST_DEFINE_LOADER(dr::ModelPtr, Model);


//////////////////////////

dust::Result<std::string> dio::_load_file_text(const dust::io::Path &path) {
    std::ifstream in(path, std::ios::in | std::ifstream::binary | std::ios::ate);
    in.exceptions(std::ios::badbit | std::ios::failbit);
    if (in)
    {
        std::ifstream::pos_type fileSize = in.tellg();
        in.seekg(0, std::ios::beg);

        std::vector<char> bytes(fileSize);
        in.read(bytes.data(), fileSize);

        return std::string(bytes.data(), fileSize);
    }
    return {};
}

DUST_DEFINE_LOADER(std::string, File);
