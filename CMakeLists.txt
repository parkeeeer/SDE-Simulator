cmake_minimum_required(VERSION 3.20)
project(SDE-Simulator LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)





add_executable(SDE-Simulator-main main.cpp)
target_compile_options(SDE-Simulator-main PRIVATE -Wall -Wextra -Wpedantic)
#target_compile_options(SDE-Simulator-main PRIVATE -mavx2)


add_library(SDE-Simulator INTERFACE)


find_package(CUDAToolkit QUIET)

if(CUDAToolkit_FOUND)
  set(USE_CUDA ON)
else()
  set(USE_CUDA OFF)
endif()

if(APPLE)
  set(USE_METAL ON)
  find_library(METAL_LIBRARY Metal)
  find_library(FOUNDATION_LIBRARY Foundation)
else()
  set(USE_METAL OFF)
endif()


add_subdirectory(src)


target_link_libraries(SDE-Simulator INTERFACE SDE-Frontend SDE-utils SDE-engine)



target_link_libraries(SDE-Simulator-main PRIVATE SDE-Simulator)



include(CTest)
if(BUILD_TESTING)
  include(FetchContent)

  FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
  )
  
  FetchContent_GetProperties(doctest)
  if(NOT doctest_POPULATED)
    FetchContent_Populate(doctest)
  endif()

  
  add_library(doctest INTERFACE
          include/results.h)
  add_library(doctest::doctest ALIAS doctest)
  target_include_directories(doctest INTERFACE ${doctest_SOURCE_DIR})

  
  list(APPEND CMAKE_MODULE_PATH ${doctest_SOURCE_DIR}/scripts/cmake)
  include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)

  add_subdirectory(Tests)
endif()


