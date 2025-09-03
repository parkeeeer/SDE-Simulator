cmake_minimum_required(VERSION 3.20)
project(SDE-Simulator LANGUAGES CXX)

# Language setup
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warnings for this target (add others similarly)
add_executable(SDE-Simulator main.cpp)
target_compile_options(SDE-Simulator PRIVATE -Wall -Wextra -Wpedantic)

# Your project subdirs
add_subdirectory(Lexer)
add_subdirectory(Parser)

# --- Testing & doctest (header-only, no subproject configure) ---
include(CTest)
if(BUILD_TESTING)
  include(FetchContent)

  FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
  )
  # Download sources only (no add_subdirectory -> avoids old policies)
  FetchContent_GetProperties(doctest)
  if(NOT doctest_POPULATED)
    FetchContent_Populate(doctest)
  endif()

  # Header-only target for include dirs
  add_library(doctest INTERFACE)
  add_library(doctest::doctest ALIAS doctest)
  target_include_directories(doctest INTERFACE ${doctest_SOURCE_DIR})

  # Bring in discovery macro
  list(APPEND CMAKE_MODULE_PATH ${doctest_SOURCE_DIR}/scripts/cmake)
  include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)

  add_subdirectory(Tests)
endif()

