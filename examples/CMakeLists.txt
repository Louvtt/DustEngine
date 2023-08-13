# Add example executable with main class at main.cpp by default
include(CMakePrintHelpers)
function(add_example name)
    # Parse arguments
    set(options USE_IMGUI)
    set(oneValue SOURCE_DIR INCLUDE_DIR)
    set(multipleValues FILES)
    cmake_parse_arguments(
        PARSE_ARGV 0 ADD_EXAMPLE 
        "${options}" 
        "${oneValue}" 
        "${multipleValues}" 
    )
    message(STATUS "Adding example ${name}")

    message(STATUS "Missing values for ADD_EXAMPLE : ${ADD_EXAMPLE_KEYWORDS_MISSING_VALUES}")
    # default source dir
    if(NOT DEFINED ADD_EXAMPLE_SOURCE_DIR)
        set(SOURCE_DIR ${name})
        message(STATUS "Defaulting SOURCE_DIR to ${SOURCE_DIR} for ${name}")
    else()
        set(SOURCE_DIR ${ADD_EXAMPLE_SOURCE_DIR})
    endif()
    # Default file entry
    if(NOT DEFINED "ADD_EXAMPLE_FILES")
        set(SOURCES "${SOURCE_DIR}/main.cpp")
        message(STATUS "Defaulting SOURCES to ${SOURCES} for ${name}")
    else()
        set(SOURCES ${ADD_EXAMPLE_FILES})
    endif()

    # Example executabe
    add_executable("dust_example_${name}" ${SOURCES})
    target_link_libraries("dust_example_${name}"
    PRIVATE
        dustlib
    )
    # Options
    if(ADD_EXAMPLE_USE_IMGUI)
        target_link_libraries("dust_example_${name}" PRIVATE imgui)
    endif()
    # include dir
    if(DEFINED "ADD_EXAMPLE_INCLUDE_DIR")
        target_include_directories("dust_example_${name}" PUBLIC ${ADD_EXAMPLE_INCLUDE_DIR})
    endif()
endfunction(add_example)


# Simple example
add_example(simple)
add_example(triangle)
add_example(sponza)