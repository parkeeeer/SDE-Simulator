add_library(SDE-Compiler STATIC compiler.cpp)
target_link_libraries(SDE-Compiler PUBLIC SDE-Lexer SDE-Parser)
target_include_directories(SDE-Compiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})