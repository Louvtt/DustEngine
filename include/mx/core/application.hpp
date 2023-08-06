#ifndef _MX_CORE_APPLICATION_HPP_
#define _MX_CORE_APPLICATION_HPP_

#include "types.hpp"
#include "time.hpp"
#include "window.hpp"
#include <memory>

extern int main(int argc, char** argv);

namespace mx {

class Application
{
private:
    std::string m_name;
    Time m_time;
    Scope<Window> m_window;

public:
    Application(const string& name, u16 width = 0, u16 height = 0);
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

#endif //_MX_CORE_APPLICATION_HPP_