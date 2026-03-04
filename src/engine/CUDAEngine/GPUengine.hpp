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
            nvrtcResult r1 = nvrtcCreateProgram(&program, source.c_str(), "kernel.cu", 0, nullptr, nullptr);
            if (r1 != NVRTC_SUCCESS) {
                std::cout << "error on nvrtc create program: ";
                std::cout << nvrtcGetErrorString(r1) << std::endl;
                exit(1);
            }
            const char* opts[] = {"--use_fast_math", "-I C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v13.0/include"};
            nvrtcResult r2 = nvrtcCompileProgram(program, 2, opts);
            if (r2 != NVRTC_SUCCESS) {
                size_t log_size;
                nvrtcGetProgramLogSize(program, &log_size);
                char* log = new char[log_size];
                nvrtcGetProgramLog(program, log);
                std::cout << "Compile log:\n" << log << std::endl;
                delete[] log;
                exit(1);
            }
            size_t ptx_s;
            nvrtcGetPTXSize(program, &ptx_s);
            ptx = new char[ptx_s];
            nvrtcGetPTX(program, ptx);
            CUresult r3 = cuModuleLoadDataEx(&module, ptx, 0, nullptr, nullptr);
            if (r3 != CUDA_SUCCESS) {
                std::cout << "error on cuModuleLoadDataEx: ";
                const char* errstring;
                cuGetErrorString(r3, &errstring);
                std::cout << errstring << std::endl;
                exit(1);
            }
           CUresult r4 = cuModuleGetFunction(&moduleFunction, module, "kernel");
            if (r4 != CUDA_SUCCESS) {
                std::cout << "error on cuModuleGetFunction: ";
                const char* errstring;
                cuGetErrorString(r4, &errstring);
                std::cout << errstring << std::endl;
                exit(1);
            }
        }

        //paths should be on device
        void launch(Num* paths, unsigned long long seed, Num dt, Num t0, Num initial_value, unsigned int num_paths, unsigned int num_steps){
            void* args[] = {&paths, &seed, &dt, &t0, &initial_value, &num_paths, &num_steps};
            int block_size = 256;
            int grid_size = (num_paths + block_size - 1) / block_size;

            CUresult result = cuLaunchKernel(moduleFunction, grid_size, 1, 1, block_size, 1, 1, 0, nullptr, args, nullptr);
            if (result != CUDA_SUCCESS) {
                const char* errstring;
                cuGetErrorString(result, &errstring);
                std::cout << errstring << "\n";
            }
            cuCtxSynchronize();
        }

        ~CudaProgram() {
            nvrtcDestroyProgram(&program);
            delete[] ptx;
        }
    };
}