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

    inline void print_help_exec() {
        std::cout << "Executes an SDE\n\n";
        std::cout << "Usage:\n";
        std::cout << "sde exec [flags] [params]\n\n";
        std::cout << "Required args:\n";
        std::cout << "--diffusion {STRING}: diffusion expression\n";
        std::cout << "--drift {STRING}: drift expression\n";
        std::cout << "--num-steps {INTEGER}: number of steps\n";
        std::cout << "--num-paths {INTEGER}: number of paths\n";
        std::cout << "--dt {REAL NUMBER}: delta time\n\n";
        std::cout << "Optional arguments:\n";
        std::cout << "--method {euler|em|euler-maruyama|milstein}: Method to use for simulation. Default euler\n";
        std::cout << "--backend {metal|cuda|cpu|gpu|ast|bytecode}: Backend for simulation. Default gpu\n";
        std::cout << "--seed {INTEGER}: seed for prng\n";
        std::cout << "-simd {NO ARGUMENT}: turns on simd mode(default lol)\n";
        std::cout << "--scalar {NO ARGUMENT}: turns on scalar mode\n";
        std::cout << "--initial-value {REAL NUMBER}: initial value for simulation.\n";
        std::cout << "--num-threads {INTEGER}: number of threads\n";
        std::cout << "--precision {double|float}: double or single precision";
        std::cout << "--stats {NO ARGUMENT}: turn on all stats output, makes --output-stats a required arg\n";
        std::cout << "--mean {NO ARGUMENT}: turns on mean output, makes --output-stats a required arg\n";
        std::cout << "--std {NO ARGUMENT}: turns on standard deviation output, makes --output-stats a required arg\n";
        std::cout << "--median {NO ARGUMENT}: turns on median output, makes --output-stats a required arg\n";
        std::cout << "--percentiles {NO ARGUMENT}: turn on on output for the 5th, 25th, 75th, and 95th percentiles, makes --output-stats a required arg\n";
        std::cout << "--output-stats {STRING}: path to output file for stats, if the file ending is .bin, it will output binary, .dat outputs in gnuplot structure, and otherwise csv.\n";
        std::cout << "--dump-paths {NO ARGUMENT}: turns on dump paths output, or outputs every path, makes --output a required arg.\n";
        std::cout << "--rand-sample {INTEGER}: amount of random paths to output, mutually exclusive with --dump-paths, and makes --output a required arg.\n";
        std::cout << "--output {STRING}: path to output file for paths, if file ending is .bin, output is binary, .dat will lead to gnuplot output, and otherwise csv.\n\n";
        std::cout << "Params:\n";
        std::cout << "<name>=<value> where name is a string and value is a real number(no dashes)";
        exit(0);
    }

    ExecFlags parse_exec(int argc, char** argv, bool expr_defined = false);

    template<sde::concepts::FloatingPoint Num>
    void output_exec(const ExecFlags& flags, array2d<Num>& results);
}
