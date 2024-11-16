//
// Created by louvtt on 7/10/24.
//

#ifndef DUSTINTIME_EDITOR_H
#define DUSTINTIME_EDITOR_H

#include <vector>
#include <string>

#include "dust/core/window.hpp"

#include <imgui.h>

namespace dust {
////////////////////////////////////////////////////////

class EditorTool {
private:
    std::string name;

protected:
    bool enabled;

public:
    explicit EditorTool(const std::string &name);
    virtual ~EditorTool() = default;

    virtual void render_ui() = 0;
    virtual bool is_panel_tool() const;
    void enable();
    void disable();
    bool &is_enabled();
    [[nodiscard]] std::string get_name() const;
};

////////////////////////////////////////////////////////

class Editor {
private:
    bool enabled;
    dust::Window *window;

    std::vector<EditorTool *> tools;

public:
    explicit Editor(dust::Window *window);
    ~Editor();

    void enable();
    void scale(float scaling);

    void new_frame();
    void render_ui();

    void add_tool(EditorTool *tool);
    Result<EditorTool*> get_tool(const std::string &name);
    void display_loading_frame();
};

////////////////////////////////////////////////////////
}; // namespace dust

#endif //DUSTINTIME_EDITOR_H
