cmake_minimum_required(VERSION 4.1.0)
project(SDE-Simulator)
set(CMAKE_CXX_STANDARD 17)

add_executable(SDE-Simulator main.cpp)


add_library(SDE-Library STATIC ExprParse.cpp ExprParse.hpp)

target_link_libraries(SDE-Simulator PRIVATE SDE-Library)

