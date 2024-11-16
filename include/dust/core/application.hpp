#ifndef _DUST_CORE_APPLICATION_HPP_
#define _DUST_CORE_APPLICATION_HPP_


#include "types.hpp"
#include "time.hpp"
#include "window.hpp"
#include "layer.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

#include "dust/render/renderer.hpp"

#include "dust/io/inputManager.hpp"
#include "dust/io/resourceManager.hpp"

#include "dust/editor/editor.hpp"

#include "dust/scripting/scriptingManager.hpp"

extern int main(int argc, char** argv);

namespace dust {

class Application
{
private:
    std::string m_name;
    Time m_time;
    Scope<Window> m_window;
    Scope<Renderer> m_renderer;
    Scope<InputManager> m_inputManager;
    Scope<io::ResourceManager> m_resourceManager;
    Scope<ScriptingManager> m_scriptingManager;
    Scope<Editor> m_editor;

    std::unordered_map<std::string, Layer*> m_layers;

    inline static Application* s_instance = nullptr;

    std::filesystem::path m_programPath;

public:
    explicit Application(const std::string& name, u32 width = 800u, u32 height = 600u);
    ~Application();

    [[nodiscard]] Window* getWindow() const;
    [[nodiscard]] Renderer* getRenderer() const;
    [[nodiscard]] InputManager* getInputManager() const;
    [[nodiscard]] io::ResourceManager* getResourceManager() const;
    [[nodiscard]] ScriptingManager* getScriptingManager() const;
    [[nodiscard]] Editor* getEditor() const;

    [[nodiscard]] std::filesystem::path getProgramPath() const;

    [[nodiscard]] Time getTime() const;

    static Application* Get();

    void pushLayer(Layer* layer);
    void popLayer(std::string name);

protected:
    virtual void update();
    virtual void render();

private:
    void run();
    void setProgramPath(const std::filesystem::path& path);
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
