#ifndef _DUST_IO_ASSETSMANAGER_HPP_
#define _DUST_IO_ASSETSMANAGER_HPP_

#include "../core/types.hpp"
#include "../core/application.hpp"

#include <filesystem>
#include <thread>
#include <vector>

#ifndef ASSETS_MANAGER_THREADS_COUNT
#   define ASSETS_MANAGER_THREADS_COUNT 20
#endif

namespace dust {
namespace io {

using Path = std::filesystem::path;

class AssetsManager
{
protected:
    inline static Path m_assetsDir{};
    friend int ::main(int argc, char** argv);

    inline static u32 m_usedThreads{0};
    inline static std::array<std::thread, ASSETS_MANAGER_THREADS_COUNT> m_threadPool{};

    static Path fromAssetsDir(const Path &path);

public:
    static Path getAssetsDir();

    template <typename LoadedType, typename ...Args, typename ResultType = Result<LoadedType>>
    static ResultType
    LoadSync(const Path &path, Args ...args);
    
    template <typename LoadedType, typename ...Args>
    static void LoadAsync(const Path &path, Args ...args, ResultPtr<LoadedType> result);

    static std::vector<Path> listAssetsDir(bool recursive = false);
};

}
}

//////////////////////////
/// MACROS

#define DUST_ADD_LOADER_SYNC_DEF(ResultType)         \
template<> Result<ResultType>                        \
dust::io::AssetsManager::LoadSync<ResultType>(const Path &path)

#define DUST_ADD_LOADER_SYNC_DEF_EX(ResultType, ...) \
template<> Result<ResultType>                        \
dust::io::AssetsManager::LoadSync<ResultType>(const Path &path, __VA_ARGS__)

#define DUST_ADD_LOADER_ASYNC_DEF(ResultType)        \
template<> void                                      \
dust::io::AssetsManager::LoadAsync<ResultType>(const Path &path, ResultPtr<ResultType> result)

#define DUST_ADD_LOADER_ASYNC_DEF_EX(ResultType, ...) \
template<> void                                       \
dust::io::AssetsManager::LoadAsync<ResultType>(const Path &path, __VA_ARGS__, ResultPtr<ResultType> result)

//////////////////////////



#endif //_DUST_IO_ASSETSMANAGER_HPP_