# Simple example
add_executable(mx_example_simple simple/main.cpp)
target_link_libraries(mx_example_simple
PRIVATE
    mxlib
)