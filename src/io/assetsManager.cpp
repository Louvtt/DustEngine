#include "dust/io/assetsManager.hpp"
#include <filesystem>
#include <vector>

namespace dio = dust::io;

dio::Path dio::AssetsManager::getAssetsDir()
{
    return m_assetsDir;
}

dio::Path dio::AssetsManager::fromAssetsDir(const Path &path)
{
    return m_assetsDir / path;
}

std::vector<dio::Path> 
dio::AssetsManager::listAssetsDir(bool recursive)
{
    std::vector<dio::Path> result{};
    std::vector<dio::Path> toScan{m_assetsDir};

    while(!toScan.empty()) {
        const dio::Path &currentDirectory = toScan.back();
        toScan.pop_back(); // remove last
        for(const auto& entry : std::filesystem::directory_iterator(currentDirectory))
        {
            if(entry.is_directory()) {
                toScan.push_back(m_assetsDir);
            } else {
                result.push_back(entry.path());
            }
        }
    }

    return result;
}
