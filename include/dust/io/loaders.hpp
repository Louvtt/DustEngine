#ifndef _DUST_IO_IMAGELOADER_HPP_
#define _DUST_IO_IMAGELOADER_HPP_

#include "dust/io/assetsManager.hpp"

#include "dust/render/material.hpp"
#include "dust/render/model.hpp"
#include "dust/render/texture.hpp"


using namespace dust;
namespace dr = dust::render;

namespace dust::io {

//////////////////////////
/// MACROS

#define DUST_FALLBACK_LOADER_KEY "."
#define DUST_DECLARE_LOADER(ResultType, Name)       \
dust::Result<ResultType>                        \
Load##Name(const dust::io::Path &path);         \
static std::unordered_map<std::string, std::function<dust::Result<ResultType>(const dust::io::Path &)>> loaders##Name =

#define DUST_DEFINE_LOADER(ResultType, Name)                        \
dust::Result<ResultType> dio::Load##Name(const dio::Path &_path) { \
    DUST_PROFILE_SECTION("io::Load" #Name);                         \
    auto path = AssetsManager::FromAssetsDir(_path);                \
    if(!fs::exists(path)) {                                         \
        DUST_ERROR("[" #Name "] {} doesn't exists.", path.string());\
        return nullptr;                                             \
    }                                                               \
    const auto loader = loaders##Name.find(path.extension().string());\
    if (loader != loaders##Name.end()) {                              \
        return loader->second(path);                                 \
    } else {                                                         \
        const auto fallbackLoader = loaders##Name.find(DUST_FALLBACK_LOADER_KEY);\
        if (fallbackLoader != loaders##Name.end()) {                  \
            return fallbackLoader->second(path);                     \
        }                                                            \
        DUST_ERROR("No " #Name " loader found");                     \
        return {};                                                   \
    }                                                                \
}

//////////////////////////



dust::Result<dr::TexturePtr> _load_texture_general(const dust::io::Path &path);
DUST_DECLARE_LOADER(dr::TexturePtr, Texture2D)
{
    {DUST_FALLBACK_LOADER_KEY, _load_texture_general}
};

dust::Result<dr::ModelPtr> _load_model_general(const dust::io::Path &path);
dust::Result<dr::ModelPtr> _load_model_gltf(const dust::io::Path &path);
DUST_DECLARE_LOADER(dr::ModelPtr, Model)
{
    { ".gltf", _load_model_gltf },
    { DUST_FALLBACK_LOADER_KEY, _load_model_general }
};


dust::Result<std::string> _load_file_text(const dust::io::Path &path);
DUST_DECLARE_LOADER(std::string, File)
{
    { DUST_FALLBACK_LOADER_KEY, _load_file_text }
};

}


#endif //_DUST_IO_IMAGELOADER_HPP_