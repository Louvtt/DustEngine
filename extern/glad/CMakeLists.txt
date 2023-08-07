add_library(glad
    "${CMAKE_CURRENT_SOURCE_DIR}/include/glad/gl.h"
    src/gl.c
)

find_package(OpenGL REQUIRED)
if(OpenGL_FOUND)
    target_link_libraries(glad PRIVATE OpenGL::GL)
endif()
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
