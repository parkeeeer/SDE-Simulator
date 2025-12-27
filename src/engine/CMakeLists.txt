add_subdirectory(CPUengine)
#add_subdirectory(GPUengine)

add_library(SDE-engine STATIC
        dispatch.cpp
)

target_include_directories(SDE-engine PUBLIC ${CMAKE_SOURCE_DIR}/include
                                      PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(SDE-engine PUBLIC SDE-CPU-engine PRIVATE SDE-AST_Analysis)