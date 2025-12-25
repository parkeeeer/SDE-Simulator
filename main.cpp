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
    config.diffusion = "0";
    config.drift = "1";
    config.safe = true;
    config.initial_value = 0.0;
    config.method = sde::Method::EULER;
    auto results = sde::bytecode_dispatch<float>(config);

    std::cout << "results: " << std::endl;
    for (size_t i = 0;i < config.num_steps;++i) {
        for (size_t j = 0;j < config.num_paths;++j) {
            std::cout << results[i * config.num_paths + j] << ' ';
        }
        std:: cout << '\n';
    }
    return 0;
}