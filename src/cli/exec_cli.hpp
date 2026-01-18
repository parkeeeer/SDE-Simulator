#pragma once


#include <charconv>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include "frontend.hpp"
#include <Variable_Environment.hpp>
#include <concepts.hpp>
#include <optional>
#include <iostream>
#include <fstream>
#include <random>
#include "get_value.hpp"

#include "results.h"


/*
 * LIST OF FLAGS:
 * -help or -h
 * --num-steps or -ns {INTEGER} REQUIRED
 * --num-paths or -np {INTEGER} REQUIRED
 * -method {euler|em|euler-maruyama|milstein} (default euler)
 * -dt {REAL NUMBER} REQUIRED
 * --initial-value or -x0 {REAL NUMBER} (default 0)
 * --num-threads or -nt {INTEGER} (default max)
 * -diff | --diffusion {EXPRESSION} REQUIRED
 * --drift {EXPRESSION} REQUIRED
 * -simd OR -scalar (default simd)
 * --precision {float|double} (default float)
 * --backend or -b {metal|cuda|cpu|gpu|ast|bytecode} (gpu picks based on system, and default is gpu, also cpu = bytecode)
 * --output or -o {FILENAME} (default stdout) (if .bin file is used will output in binary, otherwise csv style)
 * --output-stats or -os {FILENAME} (default stdout) (will also use binary if .bin file)
 * --dump-paths or -dp (makes --output require a file not stdout) (mutually exclusive with --rand-sample)
 * --percentiles or -p
 * --median or -m
 * --standard-dev or -std
 * --mean or -mean
 * --stats or -s (combines -percentiles, -median, and -std)
 * --rand-sample or -rs {INTEGER} (makes --output a required flag) (mutually exclusive with --dump-paths)
 * --sample-interval or -si {INTEGER} (default 1)
 * --seed {INTEGER} (default std::random_device)
 * ANYTHING ELSE GETS INTERPERETED AS A PARAMETER!!!!!
 */


namespace sde::cli {

    enum class Backend {
        BYTECODE,
        AST,
        GPU
    };

    enum class Method {
        EULER,
        MILSTEIN
    };

    enum class Precision {
        FLOAT,
        DOUBLE
    };

    enum class DumpFormat {
        CSV,
        BINARY
    };

    struct ExecFlags {

        //required flags
        std::string diffusion;
        std::string drift;
        size_t num_steps;
        size_t num_paths;
        double dt;


        //optional flags(have defaults)
        double initial_value = 0;
        size_t num_threads = std::thread::hardware_concurrency();
        bool simd = true;
        Precision precision = Precision::FLOAT;
        Backend backend = Backend::GPU;
        size_t sample_interval = 1;
        Method method = Method::EULER;
        size_t seed = std::random_device{}();

        //output flags
        std::optional<std::string> output_stats;
        std::optional<std::string> output_file;
        bool dump_paths = false;
        std::optional<size_t> num_rand_samples;
        bool compute_median = false;
        bool compute_std = false;
        bool compute_percentiles = false;
        bool compute_mean = false;

        sde::frontend::Environment<double> env;
    };

    inline void print_help() {
        //TODO: make an actual help message
        std::cerr << "helpful message here";
        exit(0);
    }

    ExecFlags parse_exec(int argc, char** argv, bool expr_defined = false);

    template<sde::concepts::FloatingPoint Num>
    void output_exec(const ExecFlags& flags, array2d<Num>& results);
}
