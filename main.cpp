#include<iostream>

#include "dispatch.hpp"
#include "allocator.hpp"




int main() {
    sde::Config config;
    config.use_simd = true;
    //config.threads = 2;
    config.num_steps = 100;
    config.num_paths = 100;
    config.dt = .1;
    config.initial_value = 0;
    config.diffusion = "1";
    config.drift = "0";
    config.safe = true;
    config.initial_value = 0.0;
    config.method = sde::Method::MILSTEIN;

    auto results = sde::bytecode_dispatch<double>(config);


    std::cout << "results: " << std::endl;
    for (size_t i = 0;i < config.num_steps;++i) {
        std::cout << "path " << i << ": ";
        for (size_t j = 0;j < config.num_paths;++j) {
            std::cout << results[i][j] << ' ';
        }
        std:: cout << '\n';
    }
    return 0;
}