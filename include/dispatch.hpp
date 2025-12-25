#pragma once
#include <thread>
#include "allocator.hpp"
#include "frontend.hpp"


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

        uint64_t seed = 12345;
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


    template<concepts::FloatingPoint Num>
    memory::aligned_vector<Num> bytecode_dispatch(Config& config);

    template<concepts::FloatingPoint Num>
    memory::aligned_vector<Num> AST_dispatch(Config& config);

    template<concepts::FloatingPoint Num>
    memory::aligned_vector<Num> GPU_dispatch(Config& config);
}
