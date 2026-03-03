add_executable(SDE-CLI cli.cpp)

add_library(SDE-CLI-LIB STATIC exec_cli.cpp run_cli.cpp cache_manip.cpp sql_cache.cpp)
target_link_libraries(SDE-CLI-LIB PUBLIC SDE-Simulator)

find_package(SQLite3 REQUIRED)
target_link_libraries(SDE-CLI-LIB PRIVATE SQLite::SQLite3)

target_link_libraries(SDE-CLI PUBLIC SDE-CLI-LIB)