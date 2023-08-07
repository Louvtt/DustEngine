set(HEADER_LIST
    "${DustEngine_SOURCE_DIR}/include/dust/dust.hpp"
    # Core
    "${DustEngine_SOURCE_DIR}/include/dust/core/types.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/core/platform.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/core/log.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/core/time.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/core/window.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/core/application.hpp"
    # Render
    "${DustEngine_SOURCE_DIR}/include/dust/render/renderAPI.hpp"
    "${DustEngine_SOURCE_DIR}/include/dust/render/renderer.hpp"
)

add_library(dustlib
    ${HEADER_LIST}
    # Sources
    core/application.cpp
    core/entryPoint.cpp
    core/window.cpp
)

target_include_directories(dustlib PUBLIC "${DustEngine_SOURCE_DIR}/include")

find_package(OpenGL REQUIRED)
target_link_libraries(dustlib 
PUBLIC
    spdlog
    yaml-cpp
    glm
    stdc++fs

PRIVATE
    imgui
    glad
    OpenGL::GL
)

target_compile_features(dustlib PUBLIC cxx_std_20)

# -fPIC
set_property(TARGET dustlib PROPERTY POSITION_INDEPENDENT_CODE ON)

if(NOT EMSCRIPTEN)
    target_link_libraries(dustlib PUBLIC glfw)
else()
    target_link_options(dustlib PUBLIC -sUSE_GLFW=3)
endif()


source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header files"
    FILES ${HEADER_LIST}
)
