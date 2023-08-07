#include "dust/dust.hpp"

class SimpleApp
: public dust::Application
{
public:
    SimpleApp() : dust::Application("Simple")
    { }
};

DUST_SIMPLE_ENTRY(SimpleApp)

