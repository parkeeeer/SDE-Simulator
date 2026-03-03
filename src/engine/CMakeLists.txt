add_subdirectory(CPUengine)
if(USE_CUDA)
    add_subdirectory(CUDAEngine)
endif()

if(USE_METAL)
    add_subdirectory(MetalEngine)
endif()

add_library(SDE-engine STATIC
        dispatch.cpp
)

if(USE_CUDA)
    target_compile_definitions(SDE-engine PUBLIC HAS_CUDA=1)
else()
    target_compile_definitions(SDE-engine PUBLIC HAS_CUDA=0)
endif()

if(USE_METAL)
    target_compile_definitions(SDE-engine PUBLIC HAS_METAL=1)
else()
    target_compile_definitions(SDE-engine PUBLIC HAS_METAL=0)
endif()



target_include_directories(SDE-engine PUBLIC ${CMAKE_SOURCE_DIR}/include
                                      PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(SDE-engine PUBLIC SDE-CPU-engine SDE-AST_Analysis)

if(USE_CUDA OR USE_METAL)
    target_link_libraries(SDE-engine PUBLIC SDE-GPU-engine)
endif()
