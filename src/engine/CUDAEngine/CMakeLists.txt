add_library(SDE-GPU-engine INTERFACE)

target_link_libraries(SDE-GPU-engine INTERFACE CUDA::cudart CUDA::nvrtc)

