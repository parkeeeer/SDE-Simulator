add_library(SDE-Compiler INTERFACE)
target_link_libraries(SDE-Compiler INTERFACE SDE-Lexer SDE-Parser)
target_include_directories(SDE-Compiler PUBLIC ${CMAKE_SOURCE_DIR}/Parser)