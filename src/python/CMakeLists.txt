include(FetchContent)

FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11
        GIT_TAG        v2.11.1
)

FetchContent_MakeAvailable(pybind11)

pybind11_add_module(sde_simulator bindings.cpp)

target_link_libraries(sde_simulator PRIVATE SDE-Simulator)

