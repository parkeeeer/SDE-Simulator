add_subdirectory(Compiler)
add_subdirectory(Parser)
add_subdirectory(Lexer)


add_library(SDE-Frontend INTERFACE)
target_link_libraries(SDE-Frontend INTERFACE SDE-Compiler SDE-Parser SDE-Lexer)
target_include_directories(SDE-Frontend INTERFACE ${CMAKE_SOURCE_DIR}/include)