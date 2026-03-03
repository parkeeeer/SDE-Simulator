#include "cli.hpp"

#include <chrono>


int main(int argc, char ** argv) {
    try {
        sde::cli::command cmd = sde::cli::parse_cmdline(argc, argv);
        std::visit(sde::cli::CommandExecutor{}, cmd);
    } catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }

    return 0;
}