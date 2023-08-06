#include "mx/core/application.hpp"

int main(int argc, char* argv[])
{
    mx::Scope<mx::Application> app = mx::applicationEntry(argc, argv);
    app->run();
    app.reset();
    return 0;
}