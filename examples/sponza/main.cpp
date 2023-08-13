#include "dust/dust.hpp"

class SimpleApp
: public dust::Application
{
private:

public:
    SimpleApp()
    : dust::Application("Sponza")
    { 
        
    }
};

DUST_SIMPLE_ENTRY(SimpleApp)

