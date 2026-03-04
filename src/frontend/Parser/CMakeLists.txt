add_library(SDE-Parser STATIC AST.cpp Parser.cpp)
target_include_directories(SDE-Parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(SDE-Parser 
PUBLIC  SDE-Lexer
PUBLIC  SDE-utils
PRIVATE SDE_SIMD
)



