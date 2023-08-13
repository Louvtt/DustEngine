# Add example executable with main class at main.cpp by default
function(add_example name)
    # Parse arguments
    set(options USE_IMGUI)
    set(oneValue SOURCE_DIR INCLUDE_DIR ASSETS_DIR)
    set(multipleValues FILES)
    cmake_parse_arguments(
        PARSE_ARGV 0 ADD_EXAMPLE 
        "${options}" 
        "${oneValue}" 
        "${multipleValues}" 
    )
    message(STATUS "Adding example ${name}")
    # default source dir
    if(NOT DEFINED ADD_EXAMPLE_SOURCE_DIR)
        set(SOURCE_DIR ".")
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

    # set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples/${name}")

    # Example executabe
    set(EXAMPLE_NAME "dust_example_${name}")
    add_executable(${EXAMPLE_NAME} ${SOURCES})
    target_link_libraries(${EXAMPLE_NAME}
    PRIVATE
        dustlib
    )
    set(EXAMPLE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    set(EXAMPLE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

    # Options
    if(ADD_EXAMPLE_USE_IMGUI)
        target_link_libraries(${EXAMPLE_NAME} PRIVATE imgui)
    endif()

    # include dir
    if(DEFINED "ADD_EXAMPLE_INCLUDE_DIR")
        target_include_directories(${EXAMPLE_NAME} PUBLIC ${ADD_EXAMPLE_INCLUDE_DIR})
    endif()
    
    # ASSETS
    if(DEFINED "ADD_EXAMPLE_ASSETS_DIR")
        message(STATUS "Copying ${EXAMPLE_SOURCE_DIR}/${ADD_EXAMPLE_ASSETS_DIR} into ${EXAMPLE_BINARY_DIR}/${ADD_EXAMPLE_ASSETS_DIR}")
        add_custom_target(copy-assets-files ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory 
            "${EXAMPLE_SOURCE_DIR}/${ADD_EXAMPLE_ASSETS_DIR}"
            "${EXAMPLE_BINARY_DIR}/${ADD_EXAMPLE_ASSETS_DIR}"
            # DEPENDS ${EXAMPLE_NAME}
        )
    endif()
    
endfunction(add_example)
