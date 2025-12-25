include(CTest)
enable_testing()

add_executable(LexerTests
  ${CMAKE_CURRENT_SOURCE_DIR}/LexerTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
)

target_link_libraries(LexerTests PRIVATE doctest::doctest)
target_link_libraries(LexerTests PRIVATE SDE-Lexer)
target_compile_features(LexerTests PRIVATE cxx_std_23)

doctest_discover_tests(LexerTests TEST_PREFIX "lexer.")


add_executable(ParserTests
  ${CMAKE_CURRENT_SOURCE_DIR}/ParserTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
)

target_link_libraries(ParserTests PRIVATE doctest::doctest)
target_link_libraries(ParserTests PUBLIC SDE-Parser)
target_compile_features(ParserTests PRIVATE cxx_std_23)

doctest_discover_tests(ParserTests TEST_PREFIX "parser.")

add_executable(CompilerTests
  ${CMAKE_CURRENT_SOURCE_DIR}/CompilerTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
)

target_link_libraries(CompilerTests PRIVATE doctest::doctest)
target_link_libraries(CompilerTests PUBLIC SDE-Compiler)
target_compile_features(CompilerTests PRIVATE cxx_std_23)

doctest_discover_tests(CompilerTests TEST_PREFIX "compiler.")


add_executable(DerivativeTests
    ${CMAKE_CURRENT_SOURCE_DIR}/DerivativeTests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
)


target_link_libraries(DerivativeTests PRIVATE doctest::doctest)
target_link_libraries(DerivativeTests PUBLIC SDE-Frontend SDE-AST_Analysis)
target_compile_features(DerivativeTests PRIVATE cxx_std_23)

doctest_discover_tests(DerivativeTests TEST_PREFIX "derivative.")