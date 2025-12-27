#pragma once


#include <charconv>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <frontend.hpp>
#include <Variable_Environment.hpp>
#include "io.hpp"
#include <concepts.hpp>
#include <optional>
#include <iostream>
#include <fstream>


/*
 * LIST OF FLAGS:
 * -help or -h
 * --num-steps or -ns {INTEGER} REQUIRED
 * --num-paths or -np {INTEGER} REQUIRED
 * -method {euler|em|euler-maruyama|milstein} (default euler)
 * -dt {REAL NUMBER} REQUIRED
 * --initial-value or -x0 {REAL NUMBER} REQUIRED
 * --num-threads or -nt {INTEGER} (default max)
 * -diff | --diffusion {EXPRESSION} REQUIRED
 * --drift {EXPRESSION} REQUIRED
 * -simd OR -scalar (default simd)
 * --precision {float|double} (default float)
 * --backend or -b {metal|cuda|cpu|gpu|ast|bytecode} (gpu picks based on system, and default is gpu, also cpu = bytecode)
 * --output or -o {FILENAME}
 * --output-stats or -os {FILENAME} (default stdout)
 * --dump-paths or -dp {csv|binary} (default binary)
 * --percentiles or -p
 * --median or -m
 * --standard-dev or -std
 * --mean or -mean
 * --stats or -s (combines -percentiles, -median, and -std)
 * --rand-sample or -rs {INTEGER}
 * --sample-interval or -si {INTEGER} (default 1)
 * ANYTHING ELSE GETS INTERPERETED AS A PARAMETER!!!!!
 */


namespace sde::io {

    enum class Backend {
        BYTECODE,
        AST,
        CUDA,
        METAL
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
        double initial_value;

        //optional flags(have defaults)
        size_t num_threads = std::thread::hardware_concurrency();
        bool simd = true;
        Precision precision = Precision::DOUBLE;
        Backend backend = Backend::BYTECODE; //TODO: switch to GPU whenever that gets implemented
        size_t sample_interval = 1;
        Method method = Method::EULER;

        //output flags
        std::optional<std::string> output_stats;
        std::optional<std::string> output_file;
        std::optional<DumpFormat> dump_format;
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

    inline std::string_view get_value(std::string_view input, int& i, int argc, char** argv) {
        size_t equal_pos = input.find('=');
        if (equal_pos != std::string::npos) {
            return input.substr(equal_pos + 1);
        }
        if (i + 1 < argc) {
            return argv[++i];
        }
        throw std::runtime_error("Missing value for flag: " + std::string(input));
    }

    ExecFlags parse_exec(int argc, char** argv);

    template<sde::concepts::FloatingPoint Num>
    void output_exec(const ExecFlags& flags, const std::vector<Num>& results, std::ostream& out = std::cout);
}
