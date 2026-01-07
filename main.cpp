#include<iostream>

#include "dispatch.hpp"
#include <CudaCodegen.hpp>




int main() {
    sde::Config config;
    config.diffusion = "sqrt(x*t)";
    config.drift = "t";
    config.num_paths = 100;
    config.num_steps = 1000;
    config.dt = .1;
    //config.precision = sde::precision_level::single_precision;

    sde::array2d<float> results = sde::GPU_dispatch<float>(config);
    for (size_t i = 0; i < config.num_paths; ++i) {
        std::cout << "path " << i << ": ";
        for (size_t j = 0; j < config.num_steps; ++j) {
            std::cout << results(i, j) << ' ';
        }
        std::cout << '\n';
    }

    return 0;
}