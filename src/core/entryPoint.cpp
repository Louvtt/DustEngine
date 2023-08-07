#include "dust/core/application.hpp"
#include "dust/core/log.hpp"

int main(int argc, char* argv[])
{
    DUST_INFO("Creating app");
    dust::Scope<dust::Application> app = dust::applicationEntry(argc, argv);
    DUST_INFO("Running app");
    app->run();
    DUST_INFO("Closing app");
    app.reset();
    DUST_INFO("Program ended.");
    return 0;
}