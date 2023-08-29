set(HEADER_LIST
    "${DustEngine_SOURCE_DIR}/include/dust/dust.hpp"
    # Core
    "${DustEngine_SOURCE_DIR}/include/dust/core/types.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/core/platform.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/core/log.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/core/time.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/core/window.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/core/application.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/core/layer.hpp"
    # Render
    "${DustEngine_SOURCE_DIR}/include/dust/render/renderAPI.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/render/renderer.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/render/shader.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/render/mesh.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/render/model.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/render/texture.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/render/material.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/render/skybox.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/render/framebuffer.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/render/light.hpp"
    # IO
    "${DustEngine_SOURCE_DIR}/include/dust/io/loaders.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/io/assetsManager.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/io/inputManager.hpp"
)

add_library(dustlib
    ${HEADER_LIST}
    # Sources
    core/application.cpp
    core/entryPoint.cpp
    core/window.cpp
    core/layer.cpp

    render/renderer.cpp
    render/shader.cpp
    render/mesh.cpp
    render/model.cpp
    render/camera.cpp
    render/texture.cpp
    render/material.cpp
    render/skybox.cpp
    render/framebuffer.cpp
    render/light.cpp

    io/loaders.cpp
    io/assetsManager.cpp
    io/inputManager.cpp
)

target_include_directories(dustlib PUBLIC "${DustEngine_SOURCE_DIR}/include")

find_package(OpenGL REQUIRED)
target_link_libraries(dustlib 
PUBLIC
    spdlog
    yaml-cpp
    glm
    stdc++fs
    imgui

PRIVATE
    stb
    glad
    OpenGL::GL
    assimp
)

target_compile_features(dustlib PUBLIC cxx_std_20)

# -fPIC
set_property(TARGET dustlib PROPERTY POSITION_INDEPENDENT_CODE ON)

if(NOT EMSCRIPTEN)
    target_link_libraries(dustlib PUBLIC glfw)
else()
    target_link_options(dustlib PUBLIC -sUSE_GLFW=3)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(dustlib PRIVATE "_DEBUG")
endif()


source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header files"
    FILES ${HEADER_LIST}
)
