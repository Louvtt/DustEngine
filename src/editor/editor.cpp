//
// Created by louvtt on 7/10/24.
//

#include "dust/editor/editor.hpp"

#include "IconsFontAwesome5.h"

#include "dust/core/log.hpp"
#include "dust/core/types.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include <filesystem>

using namespace dust;

Editor::Editor(dust::Window* p_window)
: window(p_window), enabled(false)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io       = ImGui::GetIO();;
    ImGuiStyle &style = ImGui::GetStyle();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Style
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.FontGlobalScale = 2.0F;
    auto scale = 2.0F;
    style.ScaleAllSizes(scale);
    io.DisplayFramebufferScale = ImVec2(scale, scale);

    // Font
    io.Fonts->AddFontDefault();
    float baseFontSize = 13.0f; // 13.0f is the size of the default font. Change to the font size you use.
    float iconFontSize = baseFontSize * 2.0f / 3.0f; // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly

    // merge in icons from Font Awesome
    if (std::filesystem::exists("assets/fonts/" FONT_ICON_FILE_NAME_FAS)) {
        static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
        ImFontConfig icons_config;
        icons_config.MergeMode        = true;
        icons_config.PixelSnapH       = true;
        icons_config.GlyphMinAdvanceX = iconFontSize;
        io.Fonts->AddFontFromFileTTF("assets/fonts/" FONT_ICON_FILE_NAME_FAS, iconFontSize,
                                     &icons_config, icons_ranges);
    } else {
        DUST_WARN("[Editor] Cannot load awesome font file");
    }

    // Setup Platform/Renderer backends
    const char* glsl_version = "#version 410";
    if(!ImGui_ImplGlfw_InitForOpenGL(window->getNativeWindow(), false)) {
        DUST_ERROR("[GLFW][ImGui] Failed to load ImGui for GLFW/OpenGL3.");
    } else {
        DUST_INFO("[GLFW][ImGui] Loaded ImGui for GLFW/OpenGL3.");
    }
    // Query device driver information
    // const char* shading_version = (const
    // char*)glGetString(GL_SHADING_LANGUAGE_VERSION); DUST_DEBUG("[OpenGL]
    // Shading Language version : {}", shading_version);
    if (!ImGui_ImplOpenGL3_Init("#version 410 core")) {
        DUST_ERROR("[OpenGL][ImGui] Failed to load ImGui for OpenGL.");
    } else {
        DUST_INFO("[OpenGL][ImGui] Loaded ImGui for OpenGL.");
    }
}

Editor::~Editor() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Editor::add_tool(EditorTool *tool) {
    tools.push_back(tool);
}

Result<EditorTool*> Editor::get_tool(const std::string &name) {
    auto found = std::find_if(tools.begin(), tools.end(), [&](EditorTool *tool) {
       if(tool == nullptr) return false;
       return tool->get_name() == name;
    });
    if (found != tools.end()) return (*found);
    else return {};
}

void Editor::new_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

void Editor::display_loading_frame() {
    new_frame();

    ImGui::Text("Loading...");
    ImGui::GetForegroundDrawList()->AddText(ImVec2(0, 0), IM_COL32(255, 255, 255, 255), "Loading...");
    ImGui::Render();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void Editor::render_ui() {
    if (ImGui::Begin("DevTools", &enabled, ImGuiWindowFlags_MenuBar)) {
        // Tools
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Tools")) {
                for (auto &tool: tools) {
                    if (ImGui::MenuItem(tool->get_name().c_str())) {
                        tool->enable();
                    }
                }
                if (ImGui::MenuItem("Close")) {
                    enabled = false;
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Panels
        if (ImGui::BeginTabBar("##tools", ImGuiTabBarFlags_Reorderable)) {
            for (auto &tool: tools) {
                if(!tool->is_panel_tool()) {
                    if (ImGui::BeginTabItem(tool->get_name().c_str(), &tool->is_enabled())) {
                        tool->render_ui();
                        ImGui::EndTabItem();
                    }
                }
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();

    // Panels
    for (auto &tool: tools) {
        if(tool->is_panel_tool()) {
            if(ImGui::Begin(tool->get_name().c_str(), &tool->is_enabled())) {
                tool->render_ui();
            }
            ImGui::End();
        }
    }

    ImGui::ShowMetricsWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::enable() {
    enabled = true;
}

void Editor::scale(float scaling) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplayFramebufferScale = ImVec2{scaling, scaling};
}

////////////////////////////////////////

EditorTool::EditorTool(const std::string &name) : enabled(true), name(name) {}

std::string EditorTool::get_name() const {
    return name;
}

bool EditorTool::is_panel_tool() const {
    return false;
}

void EditorTool::enable() {
    enabled = true;
}

void EditorTool::disable() {
    enabled = false;
}

bool &EditorTool::is_enabled() {
    return enabled;
}
