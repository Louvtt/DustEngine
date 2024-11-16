//
// Created by lucas on 12/11/2024.
//

#ifndef DUSTENGINE_INCLUDE_DUST_EDITOR_IMGUI_EXTENSIONS_HPP
#define DUSTENGINE_INCLUDE_DUST_EDITOR_IMGUI_EXTENSIONS_HPP

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "dust/render/texture.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

namespace ImGui {

IMGUI_API bool InputMat4(const char *label, glm::mat4 value, const char *format = "%.3f",
                         ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);

IMGUI_API void TextureLabelled(const char *label, dust::render::Texture *texture);

}; // namespace ImGui

#endif // DUSTENGINE_INCLUDE_DUST_EDITOR_IMGUI_EXTENSIONS_HPP
