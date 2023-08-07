#ifndef _DUST_CORE_APPLICATION_HPP_
#define _DUST_CORE_APPLICATION_HPP_

#include "types.hpp"
#include "time.hpp"
#include "window.hpp"
#include <memory>

extern int main(int argc, char** argv);

namespace dust {

class Application
{
private:
    std::string m_name;
    Time m_time;
    Scope<Window> m_window;

public:
    Application(const string& name, u16 width = 800u, u16 height = 600u);
    ~Application();

protected:
    virtual void update();
    virtual void render();

private:
    void run();
    friend int ::main(int argc, char* argv[]);
};

extern Scope<Application> applicationEntry(int argc, char* argv[]);


}
/**
 * Simple entry macro for when you don't need the command line arguments
*/
#define DUST_SIMPLE_ENTRY(AppClassName) dust::Scope<dust::Application> dust::applicationEntry(int argc, char* argv[]) { \
    return dust::createScope<AppClassName>(); \
}

#endif //_DUST_CORE_APPLICATION_HPP_