#pragma once

#include "exec_cli.hpp"
#include <string>

/*
 * USAGE: sde run <NAME> <FLAGS>
 * basically all flags are the same as exec flags in "exec_cli.hpp"
 * just no drift or diffusion flags, instead retrieving them from the
 * cache, using the name
 */

namespace sde::cli {
    struct RunFlags{
        std::string name;
        ExecFlags exec;
    };

    RunFlags parse_run(int argc, char** argv);

    template<sde::concepts::FloatingPoint Num>
    void output_run(const RunFlags& flags, sde::array2d<Num>& results);
}