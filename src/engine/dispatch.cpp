#include "dispatch.hpp"

#include "CPUdispatch.hpp"

#include "frontend.hpp"
#include "ast-analysis.hpp"
#include "CUDAEngine/CudaCodegen.hpp"

#if HAS_CUDA
#include <CUDAEngine/GPUengine.hpp>
#include "CUDAEngine/CudaCodegen.hpp"
#include <cuda_runtime.h>
#endif
#if HAS_METAL
#include "MetalEngine/MetalCodegen.hpp"
#include "MetalEngine/MetalEngine.hpp"
#endif
using namespace sde;


namespace {
    template<concepts::fp_or_simd Num>
    void typed_bytecode_dispatch(Config& config, array2d<lane_t<Num>>& results, const size_t padded_paths) {
        frontend::AST<Num> a_ast = frontend::parse<Num>(config.drift, config.env);
        frontend::AST<Num> b_ast = frontend::parse<Num>(config.diffusion, config.env);
        frontend::Program<Num> a = frontend::compile(a_ast);
        frontend::Program<Num> b = frontend::compile(b_ast);
        if (config.safe) {
            sde::engine::CPU::BytecodeSafeEval<Num> a_funct(a);
            sde::engine::CPU::BytecodeSafeEval<Num> b_funct(b);
            if (config.method == Method::EULER) {
                sde::engine::CPU::euler_functor euler_funct;
                engine::CPU::dispatch_simulation<Num, engine::CPU::BytecodeSafeEval<Num>, engine::CPU::euler_functor>(config, results.data(), a_funct, b_funct, b_funct, euler_funct, padded_paths);
            }else if (config.method == Method::MILSTEIN){
                frontend::AST<Num> b_prime_ast = frontend::differentiate(b_ast, "x");
                frontend::AST<Num> b_prime_optimized = frontend::optimize(b_prime_ast);
                frontend::Program<Num> b_prime_prog = frontend::compile(b_prime_optimized);
                engine::CPU::BytecodeSafeEval<Num> b_prime_funct(b_prime_prog);
                engine::CPU::milstein_functor milstein_funct;
                engine::CPU::dispatch_simulation<Num, engine::CPU::BytecodeSafeEval<Num>, engine::CPU::milstein_functor>(config, results.data(), a_funct, b_funct, b_prime_funct, milstein_funct, padded_paths);
            }
        } else {
            //unsafe
            sde::engine::CPU::BytecodeEval<Num> a_funct(a);
            sde::engine::CPU::BytecodeEval<Num> b_funct(b);
            if (config.method == Method::EULER) {
                sde::engine::CPU::euler_functor euler_funct;
                sde::engine::CPU::dispatch_simulation<Num, engine::CPU::BytecodeEval<Num>, engine::CPU::euler_functor>(config, results.data(), a_funct, b_funct, b_funct, euler_funct, padded_paths);
            }else if (config.method == Method::MILSTEIN) {
                frontend::AST<Num> b_prime_ast = frontend::differentiate(b_ast, "x");
                frontend::AST<Num> b_prime_optimized = frontend::optimize(b_prime_ast);
                frontend::Program<Num> b_prime_prog = frontend::compile(b_prime_optimized);
                engine::CPU::BytecodeEval<Num> b_prime_funct(b_prime_prog);
                engine::CPU::milstein_functor milstein_funct;
                engine::CPU::dispatch_simulation<Num, engine::CPU::BytecodeEval<Num>, engine::CPU::milstein_functor>(config, results.data(), a_funct, b_funct, b_prime_funct, milstein_funct, padded_paths);
            }
        }
    }

    template<concepts::fp_or_simd Num>
    void typed_AST_dispatch(Config& config, array2d<lane_t<Num>>& results, size_t padded_paths) {
        frontend::AST<Num> a_ast = frontend::parse<Num>(config.drift, config.env);
        frontend::AST<Num> b_ast = frontend::parse<Num>(config.diffusion, config.env);
        if (config.safe) {
            engine::CPU::ASTSafeEval<Num> a_funct(a_ast);
            engine::CPU::ASTSafeEval<Num> b_funct(b_ast);
            if (config.method == Method::EULER) {
                sde::engine::CPU::euler_functor euler_funct;
                engine::CPU::dispatch_simulation<Num, engine::CPU::ASTSafeEval<Num>, engine::CPU::euler_functor>(config, results.data(), a_funct, b_funct, b_funct, euler_funct, padded_paths);
            }else if (config.method == Method::MILSTEIN) {
                frontend::AST<Num> b_prime_ast = frontend::differentiate(b_ast, "x");
                frontend::AST<Num> b_prime_optimized = frontend::optimize(b_prime_ast);
                engine::CPU::ASTSafeEval<Num> b_prime_funct(b_prime_optimized);
                engine::CPU::milstein_functor milstein_funct;
                engine::CPU::dispatch_simulation<Num, engine::CPU::ASTSafeEval<Num>, engine::CPU::milstein_functor>(config, results.data(), a_funct, b_funct, b_prime_funct, milstein_funct, padded_paths);
            }
        }else {
            //unsafe
            engine::CPU::ASTEval<Num> a_funct(a_ast);
            engine::CPU::ASTEval<Num> b_funct(b_ast);
            if (config.method == Method::EULER) {
                sde::engine::CPU::euler_functor euler_funct;
                sde::engine::CPU::dispatch_simulation<Num, engine::CPU::ASTEval<Num>, engine::CPU::euler_functor>(config, results.data(), a_funct, b_funct, b_funct, euler_funct, padded_paths);
            }else if (config.method == Method::MILSTEIN) {
                frontend::AST<Num> b_prime_ast = frontend::differentiate(b_ast, "x");
                frontend::AST<Num> b_prime_optimized = frontend::optimize(b_prime_ast);
                engine::CPU::ASTEval<Num> b_prime_funct(b_prime_optimized);
                engine::CPU::milstein_functor milstein_funct;
                engine::CPU::dispatch_simulation<Num, engine::CPU::ASTEval<Num>, engine::CPU::milstein_functor>(config, results.data(), a_funct, b_funct, b_prime_funct, milstein_funct, padded_paths);
            }
        }
    }
}

template<concepts::FloatingPoint Num>
array2d<Num> sde::bytecode_dispatch(Config& config) {
    array2d<Num> results;
    if (config.use_simd) {
        size_t simd_width = simd::simd<Num>::size;
        size_t padded_paths = ((config.num_paths + simd_width - 1) / simd_width) * simd_width;
        results = array2d<Num>(config.num_steps, padded_paths, Layout::TimeMajor, config.dt, 0, simd_width);
        typed_bytecode_dispatch<sde::simd::simd<Num>>(config, results, padded_paths);
    }else {
        results = array2d<Num>(config.num_steps, config.num_paths, Layout::TimeMajor, config.dt, 0, 1);
        typed_bytecode_dispatch<Num>(config, results, config.num_paths);
    }
    return results;
}

template<concepts::FloatingPoint Num>
array2d<Num> sde::AST_dispatch(Config& config) {
    array2d<Num> results;
    if (config.use_simd) {
        size_t simd_width = simd::simd<Num>::size;
        size_t padded_paths = ((config.num_paths + simd_width - 1) / simd_width) * simd_width;
        results = array2d<Num>(config.num_steps, padded_paths, Layout::TimeMajor, config.dt, 0, simd_width);
        typed_AST_dispatch<sde::simd::simd<Num>>(config, results, padded_paths);
    }else {
        results = array2d<Num>(config.num_steps, config.num_paths, Layout::TimeMajor, config.dt, 0, 1);
        typed_AST_dispatch<Num>(config, results, config.num_paths);
    }
    return results;
}

#if HAS_CUDA
template<concepts::FloatingPoint Num>
array2d<Num> sde::GPU_dispatch(Config& config) {
    array2d<Num> results(config.num_steps, config.num_paths, Layout::TimeMajor, config.dt, 0, 1);
    frontend::AST<Num> diff = frontend::parse<Num>(config.diffusion, config.env);
    frontend::AST<Num> drift = frontend::parse<Num>(config.drift, config.env);
    engine::GPU::CudaBuilder<Num> builder(drift, diff);
    if (config.method == Method::EULER) {
        builder.append_euler();
    }else if (config.method == Method::MILSTEIN) {
        builder.append_milstein();
    }
    engine::GPU::CudaProgram<Num> program(builder.get_source());
    Num* d_paths;
    size_t size = config.num_paths * config.num_steps * sizeof(Num);
    cudaMalloc(&d_paths, size);

    program.launch(d_paths, config.seed, config.dt, 0, config.initial_value, config.num_paths, config.num_steps);

    cudaMemcpy(results.data(), d_paths, size, cudaMemcpyDeviceToHost);

    cudaFree(d_paths);
    return results;
}
#elif HAS_METAL
template<concepts::FloatingPoint Num>
array2d<Num> sde::GPU_dispatch(Config& config) {
    if (config.precision == precision_level::double_precision) throw std::runtime_error("double precision is not supported by metal");
    array2d<Num> results(config.num_steps, config.num_paths, Layout::TimeMajor, config.dt, 0, 1);
    frontend::AST<Num> diff = frontend::parse<Num>(config.diffusion, config.env);
    frontend::AST<Num> drift = frontend::parse<Num>(config.drift, config.env);
    engine::GPU::MetalBuilder<Num> builder(drift, diff);
    if (config.method == Method::EULER) {
        builder.append_euler();
    }else if (config.method == Method::MILSTEIN) {
        builder.append_milstein();
    }
    engine::GPU::MetalProgram<Num> program(builder.get_source());
    program.launch(results.data(), config.seed, config.dt, 0, config.initial_value, config.num_paths, config.num_steps);
    return results;
}
#else
template<concepts::FloatingPoint Num>
array2d<Num> sde::GPU_dispatch(Config& config) {
    throw std::runtime_error("No supported platforms are currently available");
}
#endif

template array2d<double>
sde::bytecode_dispatch<double>(sde::Config&);

template array2d<float>
sde::bytecode_dispatch<float>(sde::Config&);

template array2d<double>
sde::AST_dispatch<double>(sde::Config&);

template array2d<float>
sde::AST_dispatch<float>(sde::Config&);


template array2d<double>
sde::GPU_dispatch<double>(sde::Config&);

template array2d<float>
sde::GPU_dispatch<float>(sde::Config&);
