add_library(nv_dds
    "${CMAKE_CURRENT_SOURCE_DIR}/nv_dds.h"
    nv_dds.cpp
)

target_include_directories(nv_dds PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")