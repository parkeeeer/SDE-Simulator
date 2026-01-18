#include "run_cli.hpp"
#include "sql_cache.hpp"

using namespace sde::cli;

RunFlags sde::cli::parse_run(int argc, char** argv) {
    RunFlags flags;
    if (argc < 3) throw std::runtime_error("run requires name argument. USAGE: sde run <NAME> <FLAGS>");
    flags.name = argv[2];
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