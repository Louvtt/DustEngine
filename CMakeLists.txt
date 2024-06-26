cmake_minimum_required(VERSION 3.16)
project(DustEngine
    VERSION 0.0.1
    DESCRIPTION "A c++ opengl engine"
    LANGUAGES CXX C    
)

option(DustEngine_BUILD_EXAMPLES "Build the engine examples" ON)
option(DustEngine_BUILD_TESTS    "Build the engine test" OFF)
option(DustEngine_BUILD_DOCS     "Build the engine documentation (Doxygen)" OFF)
option(DustEngine_PROFILING      "Enable engine profiling" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# include(CMakeDependentOption)
if(PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
    set(CMAKE_CXX_STANDARD 20) # default c++ standard
    set(CMAKE_CXX_EXTENSIONS OFF) # disable std=g++ and use std=c++
    set(CMAKE_CXX_STANDARD_REQUIRE ON) # enable stdc++fs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    set(OpenGL_GL_PREFERENCE "GLVND") # modern OpenGL

    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    if (DustEngine_BUILD_DOCS)
        find_package(DOXYGEN)
        if(DOXYGEN_FOUND)
            add_subdirectory(docs)
        else()
            message(STATUS "Doxygen not found skipping docs...")
        endif()
    endif()
endif()

# ----------------------------
# Sub directories
# ----------------------------

# libraries
add_subdirectory(extern)

# source
add_subdirectory(src)

if (DustEngine_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (DustEngine_BUILD_TESTS)
    add_subdirectory(tests)
endif()