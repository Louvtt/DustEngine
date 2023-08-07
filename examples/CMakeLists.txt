# Simple example
add_executable(dust_example_simple simple/main.cpp)
target_link_libraries(dust_example_simple
PRIVATE
    dustlib
)