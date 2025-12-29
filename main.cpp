#include<iostream>

#include "dispatch.hpp"
#include "allocator.hpp"




int main() {
#if defined(__AVX2__)
    std::cout << "AVX2 enabled" << std::endl;
#elif defined(__AVX__)
    std::cout << "AVX enabled" << std::endl;
#elif defined(__SSE4_2__)
    std::cout << "SSE4.2 enabled" << std::endl;
#elif defined(__SSE2__)
    std::cout << "SSE2 enabled" << std::endl;
#else
    std::cout << "No SIMD" << std::endl;
#endif

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
    sde::memory::aligned_vector<float> results;

    results.reserve(100);

    results = sde::bytecode_dispatch<float>(config);

    std::cout << "results: " << std::endl;
    for (size_t i = 0;i < config.num_steps;++i) {
        for (size_t j = 0;j < config.num_paths;++j) {
            std::cout << results[i * config.num_paths + j] << ' ';
        }
        std:: cout << '\n';
    }
    return 0;
}