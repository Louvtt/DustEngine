# == Glad ==
add_subdirectory(glad)

# == GLFW ==
if (NOT EMSCRIPTEN)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(glfw)
else()
   message(STATUS "Don't forget to add link option -sUSE_GLFW=3")
endif()

# == ImGui ==
add_subdirectory(imgui)

# == spdlog ==
add_subdirectory(spdlog)

# == yaml-cpp ==
add_subdirectory(yaml)

# == glm ==
add_subdirectory(glm)

## == stb ==
add_subdirectory(stb)

## == assimp ==
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(assimp)

## == sol2 ==
add_subdirectory(lua)
set(SOL2_BUILD_LUA OFF CACHE BOOL "Always build Lua, do not search for it in the system")
add_subdirectory(sol2)

## == Awesome Icon fonts cpp headers ==
add_library(IconFontCppHeaders INTERFACE)
target_include_directories(IconFontCppHeaders INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/IconFontCppHeaders")

## == tracy ==
if(DustEngine_PROFILING)
    option( TRACY_ENABLE "" ON)
    option( TRACY_ON_DEMAND "" ON)
    option(TRACY_DELAYED_INIT "" ON)
endif()
add_subdirectory(tracy)