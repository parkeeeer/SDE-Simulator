#include "run_cli.hpp"
#include "sql_cache.hpp"

using namespace sde::cli;

void print_help_run() {
    std::cout << "Executes an SDE based on info from cache\n\n";
    std::cout << "Usage:\n";
    std::cout << "sde run <name> [flags] [params]\n\n";
    std::cout << "Required args:\n";
    std::cout << "<name> {STRING}: name of expression from cache\n";
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
    std::cout << "<param>=<value> where param is a string and value is a real number(no dashes)";
}

RunFlags sde::cli::parse_run(int argc, char** argv) {
    RunFlags flags;
    if (argc < 3) throw std::runtime_error("run requires name argument. USAGE: sde run <NAME> <FLAGS>");
    flags.name = argv[2];
    if (flags.name == "--help" || flags.name == "-h") {
        print_help_run();
        exit(0);
    }
    Cache cache;
    Cache::CachedExpression a = cache.load(flags.name);
    flags.exec = parse_exec(argc, argv, true);
    flags.exec.diffusion = a.diffusion;
    flags.exec.drift = a.drift;
    return flags;
}

template<sde::concepts::FloatingPoint Num>
void sde::cli::output_run(const RunFlags& flags, sde::array2d<Num>& results) {
    output_exec(flags.exec, results);
}

template void sde::cli::output_run(const RunFlags&, sde::array2d<float>&);
template void sde::cli::output_run(const RunFlags&, sde::array2d<double>&);