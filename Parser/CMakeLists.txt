add_library(SDE-Parser STATIC AST.cpp Parser.cpp)
target_include_directories(SDE-Parser PRIVATE ${CMAKE_SOURCE_DIR}/Lexer)
target_link_libraries(SDE-Parser PUBLIC SDE-Lexer)


