add_library(SDE-Parser STATIC parser.cpp AST.cpp)
target_include_directories(SDE-Parser PRIVATE ${CMAKE_SOURCE_DIR}/Lexer)
target_link_libraries(SDE-Parser PRIVATE SDE-Lexer)
