#include "dust/core/application.hpp"
#include "dust/core/log.hpp"
#include "dust/core/profiling.hpp"
#include "dust/io/assetsManager.hpp"

#include <cstdlib>
#include <filesystem>
namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    const fs::path programPath = fs::weakly_canonical(fs::path(argv[0])).parent_path();
    dust::io::AssetsManager::m_assetsDir = programPath;
    DUST_INFO("Running in {}", programPath.string());

    DUST_INFO("Creating app");
    {
        dust::Scope<dust::Application> app = dust::applicationEntry(argc, argv);
        DUST_INFO("Running app");
        app->run();
        DUST_INFO("Closing app");
        app.reset();
        DUST_INFO("Program ended.");
    }
    return EXIT_SUCCESS;
}