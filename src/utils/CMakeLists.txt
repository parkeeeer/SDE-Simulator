add_library(SDE-utils INTERFACE ${CMAKE_SOURCE_DIR}/include/utils.hpp)

target_include_directories(SDE-utils INTERFACE 
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_SOURCE_DIR}/include
)