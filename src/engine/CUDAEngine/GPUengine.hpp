#pragma once

#include <nvrtc.h>
#include <cuda.h>
#include <string>
#include "concepts.hpp"



namespace sde::engine::GPU {
    template<concepts::FloatingPoint Num>
    class CudaProgram {
        nvrtcProgram program{};
        char* ptx;
        CUmodule module{};
        CUfunction moduleFunction{};
    public:
        explicit CudaProgram(const std::string& source) {
            nvrtcCreateProgram(&program, source.c_str(), "kernel.cu", 0, nullptr, nullptr);
            const char* opts[] = {"--gpu-architecture=compute_60", "--use_fast_math"};
            nvrtcCompileProgram(program, 2, opts);
            size_t ptx_s;
            nvrtcGetPTXSize(program, &ptx_s);
            ptx = new char[ptx_s];
            nvrtcGetPTX(program, ptx);
            cuModuleLoadDataEx(&module, ptx, 0, nullptr, nullptr);
            cuModuleGetFunction(&moduleFunction, module, ptx);
        }

        //paths should be on device
        void launch(Num* paths, unsigned long long seed, Num dt, Num t0, Num initial_value, unsigned int num_paths, unsigned int num_steps){
            void* args[] = {&paths, &seed, &dt, &t0, &initial_value, &num_paths, &num_steps};
            int block_size = 256;
            int grid_size = (num_paths + block_size - 1) / block_size;

            cuLaunchKernel(moduleFunction, grid_size, 1, 1, block_size, 1, 1, 0, nullptr, args, nullptr);

            cuCtxSynchronize();
        }

        ~CudaProgram() {
            nvrtcDestroyProgram(&program);
            delete[] ptx;
        }
    };
}