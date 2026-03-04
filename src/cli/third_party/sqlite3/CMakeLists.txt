add_library(sqlite3 STATIC sqlite3.c)
set_target_properties(sqlite3 PROPERTIES LANGUAGE C)
target_include_directories(sqlite3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})