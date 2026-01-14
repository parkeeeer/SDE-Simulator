#pragma once

#include "concepts.hpp"


namespace sde::engine::GPU {
    template<concepts::FloatingPoint Num>
    class MetalProgram {
        void* device_;
        void* command_queue_;
        void* pipeline_;
        void* library_;

    public:
        explicit MetalProgram(const std::string& source);

        void launch(Num* paths, uint64_t seed, Num dt, Num t0, Num initial_value,
                   unsigned int num_paths, unsigned int num_steps);

        ~MetalProgram();
    };
}