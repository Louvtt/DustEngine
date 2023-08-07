#include "dust/core/application.hpp"

int main(int argc, char* argv[])
{
    dust::Scope<dust::Application> app = dust::applicationEntry(argc, argv);
    app->run();
    app.reset();
    return 0;
}