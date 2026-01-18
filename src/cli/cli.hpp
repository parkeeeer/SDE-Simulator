#pragma once

#include <variant>

#include "exec_cli.hpp"
#include "run_cli.hpp"
#include "cache_manip.hpp"

#include "sql_cache.hpp"

#include "dispatch.hpp"
namespace sde::cli {
    using command = std::variant<sde::cli::ExecFlags, sde::cli::StoreFlags, sde::cli::RunFlags, sde::cli::ViewFlags, sde::cli::ListFlags, sde::cli::RemoveFlags>;

    struct run_tag{};

    inline command parse_cmdline(int argc, char** argv) {
        if (argc < 2) {
            throw std::runtime_error("need a command");
        }
        std::string cmd = argv[1];
        if (cmd == "exec" || cmd == "e") {
            return parse_exec(argc, argv);
        }else if (cmd == "store" || cmd == "s") {
            return parse_store(argc, argv);
        }else if (cmd == "load" || cmd == "run") {
            return parse_run(argc, argv);
        }else if (cmd == "view" || cmd == "v") {
            return parse_view(argc, argv);
        }else if (cmd == "list" || cmd == "ls") {
            return parse_list(argc, argv);
        }else if (cmd == "remove" || cmd == "rm") {
            return parse_remove(argc, argv);
        }else{
            throw std::runtime_error("unknown command");
        }
    }

    struct CommandExecutor {
        void operator()(const ExecFlags& flags) {
            Config con;
            con.num_paths = flags.num_paths;
            con.num_steps = flags.num_steps;
            con.seed = flags.seed;
            con.diffusion = flags.diffusion;
            con.drift = flags.drift;
            con.threads = flags.num_threads;
            con.use_simd = flags.simd;
            con.dt = flags.dt;
            con.initial_value = flags.initial_value;
            con.env = flags.env;
            if (flags.method == Method::EULER) {
                con.method = sde::Method::EULER;
            }else if (flags.method == Method::MILSTEIN) {
                con.method = sde::Method::MILSTEIN;
            }
            if (flags.precision == Precision::DOUBLE) {
                sde::array2d<double> arr;
                if (flags.backend == Backend::AST) {
                    arr = sde::AST_dispatch<double>(con);
                }else if (flags.backend == Backend::BYTECODE) {
                    arr = sde::bytecode_dispatch<double>(con);
                }else if (flags.backend == Backend::GPU){
                    arr = sde::GPU_dispatch<double>(con);
                }
                output_exec(flags, arr);
            } else {
                sde::array2d<float> arr;
                if (flags.backend == Backend::AST) {
                    arr = sde::AST_dispatch<float>(con);
                }else if (flags.backend == Backend::BYTECODE) {
                    arr = sde::bytecode_dispatch<float>(con);
                }else if (flags.backend == Backend::GPU) {
                    arr = sde::GPU_dispatch<float>(con);
                }
                output_exec(flags, arr);
            }
        }

        void operator()(const StoreFlags& flags) {
            Cache cache;
            cache.store(flags.name, flags.drift, flags.diffusion);
            output_store(flags);
        }

        void operator()(const RunFlags& flags) {
            operator()(flags.exec);
        }

        void operator()(const ViewFlags& flags) {
            output_view(flags);
        }

        void operator()(const ListFlags& flags) {
            output_list(flags);
        }

        void operator()(const RemoveFlags& flags) {
            Cache cache;
            cache.remove(flags.name);
            output_remove(flags);
        }
    };
}


