#include "exec_io.hpp"

#include <pstl/glue_execution_defs.h>


using namespace sde::io;
using namespace std;

ExecFlags sde::io::parse_exec(int argc, char** argv) {
    ExecFlags flags;
    //required definitions
    bool drift_defined = false;
    bool diffusion_defined = false;
    bool dt_defined = false;
    bool initial_value_defined = false;
    bool num_steps_defined = false;
    bool num_paths_defined = false;

    //optional definitions to stop duplicates
    bool num_threads_defined = false;
    bool simd_defined = false;
    bool precision_defined = false;
    bool backend_defined = false;
    bool output_file_defined = false;
    bool dump_format_defined = false;
    bool num_rand_samples_defined = false;
    bool percentiles_defined = false;
    bool median_defined = false;
    bool std_defined = false;
    bool mean_defined = false;
    bool stats_defined = false;
    bool sample_interval_defined = false;

    for (int i = 2;i < argc;i++) {
        std::string_view arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            print_help();
        }else if (arg.starts_with("--num-steps") || arg == "-ns") {
            if (num_steps_defined){throw std::runtime_error("num steps already defined");}
            auto value = get_value(arg, i, argc, argv);
            auto [p,ec] = std::from_chars(value.data(), value.data() + value.size(), flags.num_steps);
            if (ec != std::errc()) {throw std::runtime_error("invalid number of steps");}
            num_steps_defined = true;
        }else if (arg.starts_with("--num-paths") || arg == "-np") {
            if (num_paths_defined){throw std::runtime_error("num paths already defined");}
            auto value = get_value(arg, i, argc, argv);
            auto [p,ec] = std::from_chars(value.data(), value.data() + value.size(), flags.num_paths);
            if (ec != std::errc()) {throw std::runtime_error("invalid number of paths");}
            num_paths_defined = true;
        }else if (arg =="--dt" || arg == "-dt") {
            if (dt_defined){throw std::runtime_error("dt already defined");}
            auto value = get_value(arg, i, argc, argv);
            auto [p,ec] = std::from_chars(value.data(), value.data() + value.size(), flags.dt);
            if (ec != std::errc()) {throw std::runtime_error("invalid dt");}
            dt_defined = true;
        }else if (arg.starts_with("--initial-value") || arg == "-x0") {
            if (initial_value_defined){throw std::runtime_error("initial value already defined");}
            auto value = get_value(arg, i, argc, argv);
            auto [p,ec] = std::from_chars(value.data(), value.data() + value.size(), flags.initial_value);
            if (ec != std::errc()) {throw std::runtime_error("invalid initial value");}
            initial_value_defined = true;
        }else if (arg.starts_with("--diffusion") || arg == "-diff") {
            if (diffusion_defined){throw std::runtime_error("diffusion already defined");}
            diffusion_defined = true;
            flags.diffusion = get_value(arg, i, argc, argv);
        }else if (arg.starts_with("--drift") || arg == "-drift") {
            if (drift_defined){throw std::runtime_error("drift already defined");}
            drift_defined = true;
            flags.drift = get_value(arg, i, argc, argv);
        }else if (arg.starts_with("--num-threads") || arg == "-nt") {
            if (num_threads_defined){throw std::runtime_error("num threads already defined");}
            auto value = get_value(arg, i, argc, argv);
            auto [p,ec] = std::from_chars(value.data(), value.data() + value.size(), flags.num_threads);
            if (ec != std::errc()) {throw std::runtime_error("invalid number of threads");}
            num_threads_defined = true;
        }else if (arg == "-simd" || arg == "--simd") {
            if (simd_defined){throw std::runtime_error("simd already defined");}
            flags.simd = true;
            simd_defined = true;
        }else if (arg == "-scalar" || arg == "--scalar") {
            if (simd_defined){throw std::runtime_error("simd already defined");}
            flags.simd = false;
            simd_defined = true;
        }else if (arg == "-precision" || arg == "--precision") {
            if (precision_defined){throw std::runtime_error("precision already defined");}
            auto value = get_value(arg, i, argc, argv);
            if (value == "float") {
                flags.precision = Precision::FLOAT;
            }else if (value == "double") {
                flags.precision = Precision::DOUBLE;
            }else {
                throw std::runtime_error("invalid precision");
            }
            precision_defined = true;
        }else if (arg == "-backend" || arg == "--backend") {
            if (backend_defined){throw std::runtime_error("backend already defined");}
            auto value = get_value(arg, i, argc, argv);
            if (value == "cpu") {
                flags.backend = Backend::CPU;
            }else if (value == "cuda") {
                flags.backend = Backend::CUDA;
            }else if (value == "metal") {
                flags.backend = Backend::METAL;
            }else if (value == "gpu") {
                #ifdef __APPLE__
                    flags.backend = Backend::METAL;
                #elif defined(__CUDA_ARCH__) || defined(__NVCC__)
                    flags.backend = Backend::CUDA;
                #else
                    throw std::runtime_error("gpu backend not supported on this platform");
                #endif
            }else {
                throw std::runtime_error("invalid backend");
            }
            backend_defined = true;
        }else if (arg == "-o" || arg == "--output") {
            if (output_file_defined){throw std::runtime_error("output file already defined");}
            flags.output_file = get_value(arg, i, argc, argv);
            output_file_defined = true;
        }else if (arg == "-dp" || arg == "--dump-paths") {
            if (dump_format_defined){throw std::runtime_error("dump format already defined");}
            auto value = get_value(arg, i, argc, argv);
            if (value == "csv") {
                flags.dump_format = DumpFormat::CSV;
            }else if (value == "binary") {
                flags.dump_format = DumpFormat::BINARY;
            }
            dump_format_defined = true;
        }else if (arg == "-rs" || arg == "--rand-sample") {
            if (num_rand_samples_defined){throw std::runtime_error("random sample already defined");}
            size_t temp;
            auto value = get_value(arg, i, argc, argv);
            auto [p,ec] = std::from_chars(value.data(), value.data() + value.size(), temp);
            if (ec != std::errc()) {throw std::runtime_error("invalid number of random samples");}
            flags.num_rand_samples = temp;
            num_rand_samples_defined = true;
        }else if (arg == "-p" || arg == "--percentiles") {
            if (percentiles_defined){throw std::runtime_error("percentiles already defined");}
            flags.compute_percentiles = true;
            percentiles_defined = true;
        }else if (arg == "-m" || arg == "--median") {
            if (median_defined){throw std::runtime_error("median already defined");}
            flags.compute_median = true;
            median_defined = true;
        }else if (arg == "-std" || arg == "--standard-dev") {
            if (std_defined){throw std::runtime_error("standard deviation already defined");}
            flags.compute_std = true;
            std_defined = true;
        }else if (arg == "-mean" || arg == "--mean") {
            if (mean_defined){throw std::runtime_error("mean already defined");}
            flags.compute_mean = true;
            mean_defined = true;
        }else if (arg == "-s" || arg == "--stats") {
            if (stats_defined){throw std::runtime_error("stats already defined");}
            flags.compute_percentiles = true;
            flags.compute_median = true;
            flags.compute_std = true;
            flags.compute_mean = true;
            stats_defined = true;
        }else if (!arg.starts_with("-")) {
            size_t eq_pos = arg.find('=');
            auto var = arg.substr(0, eq_pos);
            auto value = arg.substr(eq_pos + 1);
            double numerical_value;
            std::from_chars(value.data(), value.data() + value.size(), numerical_value);
            flags.env.add_param(std::string(var), numerical_value);
        }else if (arg.starts_with("-os") || arg.starts_with("--output-stats")) {
            auto value = get_value(arg, i, argc, argv);
            flags.output_stats = value;
        }else if (arg.starts_with("-si") || arg.starts_with("--sample-interval")) {
            if (sample_interval_defined){throw std::runtime_error("sample interval already defined");}
            auto value = get_value(arg, i, argc, argv);
            size_t temp;
            auto [p,ec] = std::from_chars(value.data(), value.data() + value.size(), temp);
            if (ec != std::errc()) {throw std::runtime_error("invalid sample interval");}
            flags.sample_interval = temp;
            sample_interval_defined = true;
        }else if (arg.starts_with("-method")) {
            auto value = get_value(arg, i, argc, argv);
            if (value == "euler" || value == "em" || value == "euler-maruyama") {
                flags.method = Method::EULER;
            }else if (value == "milstein") {
                flags.method = Method::MILSTEIN;
            }
        }
    }
    if (!diffusion_defined) {throw std::runtime_error("Missing required argument: --diffusion");}
    if (!drift_defined) {throw std::runtime_error("Missing required argument: --drift");}
    if (!num_steps_defined) {throw std::runtime_error("Missing required argument: --num-steps");}
    if (!num_paths_defined) {throw std::runtime_error("Missing required argument: --num-paths");}
    if (!dt_defined) {throw std::runtime_error("Missing required argument: --dt");}
    if (!initial_value_defined) {throw std::runtime_error("Missing required argument: --initial-value");}
    if (!mean_defined && std_defined) {throw std::runtime_error("mean needed for std");}
    return flags;
}

template<sde::concepts::FloatingPoint Num>
struct Stats {
    Num mean;
    Num std;
    Num median;
    Num p5;
    Num p25;
    Num p75;
    Num p95;
};

template<sde::concepts::FloatingPoint Num>
Stats<Num> compute_stats_at_step(const ExecFlags& flags, const vector<Num>& results, size_t step) {
    Stats<Num> stats;
    const Num* values = results.data() + step * flags.num_paths;
    if (flags.compute_mean || flags.compute_std) {
        Num mean = 0;
        Num M2 = 0;
        for (size_t i = 0; i < flags.num_paths; ++i) {
            Num delta = values[i] - mean;
            mean += delta / static_cast<Num>(i + 1);
            M2 += delta * (values[i] - mean);
        }
        if (flags.compute_mean) {stats.mean = mean;}
        if (flags.compute_std) stats.std = sqrt(M2 / static_cast<Num>(flags.num_paths));
    }

    if (flags.compute_median || flags.compute_percentiles) {
        vector<Num> values_copy(values, values + flags.num_paths);
        if (flags.compute_median && !flags.compute_percentiles) {
            nth_element(values_copy.begin(), values_copy.begin() + values_copy.size() / 2, values_copy.end());
            stats.median = values_copy[values_copy.size() / 2];
        } else {
            sort(values_copy.begin(), values_copy.end());
            if (flags.compute_median) stats.median = values_copy[values_copy.size() / 2];
            stats.p5 = values_copy[values_copy.size() * 0.05];
            stats.p25 = values_copy[values_copy.size() * 0.25];
            stats.p75 = values_copy[values_copy.size() * 0.75];
            stats.p95 = values_copy[values_copy.size() * 0.95];
        }
    }
    return stats;
}

template<sde::concepts::FloatingPoint Num>
vector<Stats<Num>> compute_all_stats(const vector<Num>& results, const ExecFlags& flags) {
    size_t interval = flags.sample_interval;
    vector<size_t> steps;
    steps.reserve(flags.num_steps / interval + 1);
    for (size_t i = 0; i < flags.num_steps; i += interval) {
        steps.push_back(i);
    }
    vector<Stats<Num>> stats;
    stats.reserve(steps.size());
    transform(steps.begin(), steps.end(), back_inserter(stats), [&](const size_t step) {
        return compute_stats_at_step<Num>(flags, results, step);
    });
    return stats;
}

template<sde::concepts::FloatingPoint Num>
void output_exec_csv(const ExecFlags& flags, const vector<Num>& results, ostream& out) {
    out << "# SDE Simulation Results\n";
    out << "# num_paths: " << flags.num_paths << '\n';
    out << "# num_steps: " << flags.num_steps << '\n';
    out << "# dt: " << flags.dt << '\n';
    out << "# initial_value: " << flags.initial_value << '\n';
    out << "# drift: " << flags.drift << '\n';
    out << "# diffusion: " << flags.diffusion << '\n';
    flags.env.print_params(); out << '\n';
    out << "# paths: (column zero is time)\n";
    Num curr_time = 0.0;
    for (size_t i = 0;i < flags.num_steps;++i) {
        out << curr_time << ", ";
        for (size_t j = 0;j < flags.num_paths;++j) {
            out << results[i * flags.num_paths + j] << ", ";
        }
        out << '\n';
        curr_time += flags.dt;
    }
}

template<sde::concepts::FloatingPoint Num>
void output_exec_binary(const ExecFlags& flags, const vector<Num>& results, ostream& out) {
    // Type identifier
    uint8_t type_id = sizeof(Num) == 4 ? 1 : 2;
    out.write(reinterpret_cast<const char*>(&type_id), sizeof(uint8_t));

    out.write(reinterpret_cast<const char*>(&flags.num_paths), sizeof(size_t));
    out.write(reinterpret_cast<const char*>(&flags.num_steps), sizeof(size_t));
    out.write(reinterpret_cast<const char*>(&flags.dt), sizeof(Num));
    out.write(reinterpret_cast<const char*>(&flags.initial_value), sizeof(Num));
    size_t padded_paths = results.size() / flags.num_steps;
    for (size_t step = 0; step < flags.num_steps; ++step) {
        size_t offset = step * padded_paths;
        out.write(reinterpret_cast<const char*>(&results[offset]),
                  flags.num_paths * sizeof(Num));
    }
}



template<sde::concepts::FloatingPoint Num>
void sde::io::output_exec(const ExecFlags& flags, const vector<Num>& results, std::ostream& out) {
    vector<Stats<Num>> stats = compute_all_stats<Num>(results, flags);
    if (flags.compute_mean) {
        out << "mean:\n";
        for (const auto& stat : stats) {
            out << stat.mean << ", ";
        }
        out << '\n';
    }
    if (flags.compute_std) {
        out << "standard deviation:\n";
        for (const auto& stat : stats) {
            out << stat.std << ", ";
        }
        out << '\n';
    }
    if (flags.compute_percentiles) {
        out << "5th percentile:\n";
        for (const auto& stat : stats) {
            out << stat.p5 << ", ";
        }
        out << "\n25th percentile:\n";
        for (const auto& stat : stats) {
            out << stat.p25 << ", ";
        }
    }
    if (flags.compute_median) {
        out << "median:\n";
        for (const auto& stat : stats) {
            out << stat.median << ", ";
        }
        out << '\n';
    }
    if (flags.compute_percentiles) {
        out << "75th percentile:\n";
        for (const auto& stat : stats) {
            out << stat.p75 << ", ";
        }
        out << "\n95th percentile:\n";
        for (const auto& stat : stats) {
            out << stat.p95 << ", ";
        }
        out << "\n";
    }
    if (flags.dump_format.has_value()) {
        if (flags.dump_format == DumpFormat::CSV) {
            ofstream o(flags.output_file.value());
            output_exec_csv(flags, results, o);
        }else {
            ofstream o(flags.output_file.value(), ios::binary);
            output_exec_binary(flags, results, o);
        }
    }
}

template void output_exec<double>(const ExecFlags&, const vector<double>&, std::ostream&);
template void output_exec<float>(const ExecFlags&, const vector<float>&, std::ostream&);