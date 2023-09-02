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


public:
    static Path FromAssetsDir(const Path &path);
    static Path GetAssetsDir();

    static std::vector<Path> ListAssetsDir(bool recursive = false);
};

}
}

//////////////////////////
/// MACROS

#define DUST_ADD_LOADER(ResultType, Name)       \
dust::Result<ResultType>              \
Load##Name(const dust::io::Path &path)

#define DUST_ADD_LOADER_ASYNC(ResultType, Name) \
void Load##Name##Async(const dust::io::Path &path, dust::ResultPtr<ResultType> result)

//////////////////////////



#endif //_DUST_IO_ASSETSMANAGER_HPP_