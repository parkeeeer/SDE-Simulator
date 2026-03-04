add_subdirectory(minisimd)
add_subdirectory(frontend)
add_subdirectory(utils)
add_subdirectory(engine)

if(BUILD_CLI)
    add_subdirectory(cli)
endif()

if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
