add_library(SDE-CPU-engine INTERFACE)

target_include_directories(SDE-CPU-engine INTERFACE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(SDE-CPU-engine INTERFACE SDE-Frontend SDE-utils SDE_SIMD)