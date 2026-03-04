add_library(SDE-AST_Analysis STATIC ast-analysis.cpp)
target_include_directories(SDE-AST_Analysis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(SDE-AST_Analysis PRIVATE SDE-Parser SDE_SIMD)