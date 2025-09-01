cmake_minimum_required(VERSION 4.1.0)
project(SDE-Simulator)
set(CMAKE_CXX_STANDARD 17)

add_executable(SDE-Simulator main.cpp)




target_link_libraries(SDE-Simulator PRIVATE SDE-Library)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic ${CMAKE_CXX_FLAGS}")

add_subdirectory(Tests)
add_subdirectory(Lexer)
add_subdirectory(Parser)

