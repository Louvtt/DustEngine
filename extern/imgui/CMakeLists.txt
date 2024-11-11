cmake_minimum_required(VERSION 3.14..)
project(DearImGui
    DESCRIPTION "c++ gui immediate library"
    LANGUAGES CXX C
)

########################

set(DearImGui_WINDOW_BACKEND "glfw" CACHE STRING "DearImGui window backend api")
set(DearImGui_RENDER_BACKEND "opengl3" CACHE STRING "DearImGui render backend api")
option(DearImGui_MISC_STDLIB "DearImGui stdlib implementation" ON)

########################
# add library
add_library(imgui
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imconfig.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_internal.h"
    # stb imgui headers
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_rectpack.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_textedit.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_truetype.h"
    # Sources
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
)

target_include_directories(imgui PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends"
)

########################
# C++ STDLIB
if(DearImGui_MISC_STDLIB)
    target_sources(imgui
    PUBLIC 
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/misc/cpp/imgui_stdlib.h"
        imgui/misc/cpp/imgui_stdlib.cpp
    )
    target_include_directories(imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/imgui/misc/cpp")
endif()

########################
# Window Backend 
if(DearImGui_WINDOW_BACKEND STREQUAL "glfw")
    if(NOT EMSCRIPTEN)
        target_sources(imgui 
        PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.h"
            "imgui/backends/imgui_impl_glfw.cpp"
        )
        target_link_libraries(imgui PRIVATE glfw)
    else()
        target_link_options(imgui PRIVATE -sUSE_GLFW=3)
    endif()
else()
    message(STATUS "ImGui backend ${DearImGui_WINDOW_BACKEND} not supported.")
endif()

########################
# Render backend

if((DearImGui_RENDER_BACKEND STREQUAL "opengl2") OR (DearImGui_RENDER_BACKEND STREQUAL "opengl3"))
    find_package(OpenGL REQUIRED)
    if(OpenGL_FOUND)
        target_sources(imgui 
        PUBLIC
            "imgui/backends/imgui_impl_${DearImGui_RENDER_BACKEND}.cpp"
        )
        target_link_libraries(imgui PRIVATE OpenGL::GL)
    endif()
elseif(DearImGui_RENDER_BACKEND STREQUAL "vulkan")
    find_package(Vulkan REQUIRED)
    if(Vulkan_FOUND)
        target_sources(imgui 
        PUBLIC
            "imgui/backends/imgui_impl_vulkan.cpp"
        )
        target_link_libraries(imgui PRIVATE Vulkan::Vulkan)
    endif()
else()
    message(STATUS "ImGui backend ${DearImGui_WINDOW_BACKEND} not supported.")
endif()