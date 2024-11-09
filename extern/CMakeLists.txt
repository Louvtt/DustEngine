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
add_subdirectory(assimp)

## == nvidia ==
add_subdirectory(nvidia)

## == sol2 ==
add_subdirectory(lua)
add_subdirectory(sol2)

## == tracy ==
if(DustEngine_PROFILING)
    option( TRACY_ENABLE "" ON)
    option( TRACY_ON_DEMAND "" ON)
    option(TRACY_DELAYED_INIT "" ON)
endif()
add_subdirectory(tracy)