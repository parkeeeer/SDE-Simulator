add_subdirectory(CPUengine)
if(USE_CUDA)
    add_subdirectory(CUDAEngine)
endif()


add_library(SDE-engine STATIC
        dispatch.cpp
)

target_include_directories(SDE-engine PUBLIC ${CMAKE_SOURCE_DIR}/include
                                      PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(SDE-engine PUBLIC SDE-CPU-engine SDE-AST_Analysis)

if(USE_CUDA)
    target_link_libraries(SDE-engine PUBLIC SDE-GPU-engine)
endif()
