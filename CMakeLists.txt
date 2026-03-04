cmake_minimum_required(VERSION 3.20)
project(SDE-Simulator LANGUAGES CXX C)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)


option(BUILD_PYTHON_BINDINGS "Build Python Bindings" ON)
option(BUILD_CLI "build command line interface" ON)
option(BUILD_TESTS "build tests" OFF)

if(BUILD_PYTHON_BINDINGS)
  find_package(Python 3 COMPONENTS Interpreter Development REQUIRED)

  message(STATUS "Building Python bindings for Python ${Python_VERSION}")
endif()


add_library(SDE-Simulator INTERFACE)
target_include_directories(SDE-Simulator INTERFACE ${CMAKE_SOURCE_DIR}/include)


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


if(BUILD_TESTS)
  enable_testing()
  include(FetchContent)

  FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
  )
  
  FetchContent_MakeAvailable(doctest)

  
  list(APPEND CMAKE_MODULE_PATH ${doctest_SOURCE_DIR}/scripts/cmake)
  include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)

  add_subdirectory(Tests)
endif()


