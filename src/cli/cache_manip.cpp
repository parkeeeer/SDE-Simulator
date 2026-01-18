#include "cache_manip.hpp"

#include "get_value.hpp"

#include <string_view>
#include  <iostream>

using namespace sde::cli;

void print_help_store() {
    std::cout << "helpful message here" << std::endl;
}

StoreFlags sde::cli::parse_store(int argc, char** argv) {
    StoreFlags flags;
    bool name_defined = false;
    bool drift_defined = false;
    bool diffusion_defined = false;


    for (int i = 2; i < argc; i++) {
        std::string_view arg = argv[i];
        if (arg.starts_with("--help") || arg.starts_with("-h")) {
            print_help_store();
            exit(0);
        } else if (arg.starts_with("--name") || arg.starts_with("-n")) {
            if (name_defined) throw std::runtime_error("--name defined multiple times");
            auto val = get_value(arg, i, argc, argv);
            name_defined = true;
            flags.name = std::string(val);
        }else if (arg.starts_with("--drift")) {
            if (drift_defined) throw std::runtime_error("--drift defined multiple times");
            auto val = get_value(arg, i, argc, argv);
            drift_defined = true;
            flags.drift = std::string(val);
        }else if (arg.starts_with("--diffusion")) {
            if (diffusion_defined) throw std::runtime_error("--diffusion defined multiple times");
            auto val = get_value(arg, i, argc, argv);
            diffusion_defined = true;
            flags.diffusion = std::string(val);
        }
    }
    if (!name_defined) throw std::runtime_error("Missing required argument: --name");
    if (!drift_defined) throw std::runtime_error("Missing required argument: --drift");
    if (!diffusion_defined) throw std::runtime_error("Missing required argument: --drift");
    return flags;
}

void print_help_view() {
    std::cout << "helpful message here" << std::endl; //TODO: add helpful message
}

ViewFlags sde::cli::parse_view(int argc, char** argv) {
    ViewFlags flags;
    if (argc < 3) throw std::runtime_error("Missing Name Argument");
    std::string_view arg = argv[2];
    if (arg == "--help" || arg == "-h") {
        print_help_view();
        exit(0);
    }
    flags.name = std::string(arg);
    if (argc > 3) {
        arg = argv[3];
        if (arg == "--help" || arg == "-h") {
            print_help_view();
            exit(0);
        }
    }
    return flags;
}

void print_help_remove() {
    std::cout << "remove message here" << std::endl; //TODO: add helpful message
}

RemoveFlags sde::cli::parse_remove(int argc, char** argv) {
    RemoveFlags flags;
    if (argc < 3) throw std::runtime_error("Missing Name Argument");
    std::string_view arg = argv[2];
    if (arg == "--help" || arg == "-h") {
        print_help_remove();
        exit(0);
    }
    flags.name = std::string(arg);
    if (argc > 3) {
        arg = argv[3];
        if (arg == "--help" || arg == "-h") {
            print_help_remove();
            exit(0);
        }
    }
    return flags;
}

