macro(add_example example_name)
    add_executable("dust_example_${example_name}" "${example_name}/main.cpp")
    target_link_libraries("dust_example_${example_name}"
    PRIVATE
        dustlib
    )
endmacro(add_example)


# Simple example
add_example(simple)
add_example(triangle)
add_example(sponza)