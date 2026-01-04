add_library(SDE-GPU-engine INTERFACE)

target_link_libraries(SDE-GPU-engine INTERFACE CUDA::cudart CUDA::nvrtc CUDA::cuda_driver)

target_include_directories(SDE-GPU-engine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})