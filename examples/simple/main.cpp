#include "mx/mx.hpp"

class SimpleApp
: public mx::Application
{
public:
    SimpleApp() : mx::Application("Simple")
    { }
};

mx::Scope<mx::Application> mx::applicationEntry(int argc, char **argv)
{
    return mx::createScope<SimpleApp>();
}

