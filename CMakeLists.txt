cmake_minimum_required(VERSION 3.0)
project(MxEngine
    VERSION 0.0.1
    DESCRIPTION "A c++ opengl engine"
    LANGUAGES CXX C    
)

option(MxEngine_BUILD_EXAMPLES "Build the mx engine examples" OFF)
option(MxEngine_BUILD_TESTS    "Build the mx engine test" OFF)
option(MxEngine_BUILD_DOCS     "Build the mx engine documentation (Doxygen)" OFF)

# include(CMakeDependentOption)
if(PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
    set(CMAKE_CXX_STANDARD 20) # default c++ standard
    set(CMAKE_CXX_EXTENSIONS OFF) # disable std=g++ and use std=c++
    set(CMAKE_CXX_STANDARD_REQUIRE ON) # enable stdc++fs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    set(OpenGL_GL_PREFERENCE "GLVND") # not legacy OpenGL

    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    if (MxEngine_BUILD_DOCS)
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

if (MxEngine_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (MxEngine_BUILD_TESTS)
    add_subdirectory(tests)
endif()