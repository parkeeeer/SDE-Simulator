include(CTest)
enable_testing()

add_executable(LexerTests
  ${CMAKE_CURRENT_SOURCE_DIR}/LexerTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
)
target_include_directories(LexerTests PRIVATE ${CMAKE_SOURCE_DIR}/lexer) # so it sees Lexer/lexer.hpp
target_link_libraries(LexerTests PRIVATE doctest::doctest)
target_compile_features(LexerTests PRIVATE cxx_std_20)

doctest_discover_tests(LexerTests TEST_PREFIX "lexer.")

