add_library(SDE-GPU-engine STATIC MetalEngine.mm)

target_link_libraries(SDE-GPU-engine PRIVATE ${METAL_LIBRARY} ${FOUNDATION_LIBRARY})

target_include_directories(SDE-GPU-engine PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})