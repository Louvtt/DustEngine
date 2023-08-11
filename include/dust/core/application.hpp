#ifndef _DUST_CORE_APPLICATION_HPP_
#define _DUST_CORE_APPLICATION_HPP_

#include "types.hpp"
#include "time.hpp"
#include "window.hpp"
#include "../render/renderer.hpp"
#include <memory>

extern int main(int argc, char** argv);

namespace dust {

class Application
{
private:
    std::string m_name;
    Time m_time;
    Scope<Window> m_window;
    Scope<Renderer> m_renderer;
    
    inline static Ref<Application> s_instance = nullptr;

public:
    Application(const std::string& name, u32 width = 800u, u32 height = 600u);
    ~Application();

    const Window &getWindow() const;
    const Renderer &getRenderer() const;

    Time getTime() const;

    static Weak<Application> Get();

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