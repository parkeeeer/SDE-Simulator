add_library(SDE-IO STATIC ${CMAKE_SOURCE_DIR}/include/io.hpp exec_io.cpp io.cpp)

target_include_directories( SDE-IO
        PUBLIC ${CMAKE_SOURCE_DIR}/include
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(SDE-IO PRIVATE SDE-Frontend)