#pragma once
#include <thread> //for hardware_concurrency
#include <random> //for random_device
#include "allocator.hpp"
#include "frontend.hpp"
#include "results.h"



namespace sde {
    enum class precision_level {
        double_precision,  //uses doubles
        single_precision   //uses floats
    };

    enum class Method {
        EULER,
        MILSTEIN
    };

    struct Config {

        uint64_t seed = std::random_device{}();
        double dt;
        double initial_value = 0;
        std::string diffusion;
        std::string drift;
        size_t num_steps;
        size_t num_paths;
        precision_level precision = precision_level::double_precision;
        Method method = Method::EULER;

        frontend::Environment<double> env = frontend::Environment<double>();



        //CPU specific settings
        bool use_simd = true;
        size_t threads = std::thread::hardware_concurrency();
        bool safe = true;


        //GPU specific settings


        void add_param(const std::string& name, double value) {
            env.add_param(name, value);
        }
    };

    /*
     * These functions will return a 2d array with the simulation data for the simulation specified in config
     * all of these will return the data in a time-major format, but remember that array2d always uses <path>, <step>
     * for indexing. Also remember that metal cannot use doubles so do not use GPU_dispatch with doubles on macos
     */
    template<concepts::FloatingPoint Num>
    array2d<Num> bytecode_dispatch(Config& config);

    template<concepts::FloatingPoint Num>
    array2d<Num> AST_dispatch(Config& config);

    template<concepts::FloatingPoint Num>
    array2d<Num> GPU_dispatch(Config& config);
}
