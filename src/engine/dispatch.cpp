#include "dispatch.hpp"

#include "CPUdispatch.hpp"

#include "frontend.hpp"
#include "ast-analysis.hpp"

using namespace sde;


namespace {
    template<concepts::fp_or_simd Num>
    void typed_bytecode_dispatch(Config& config, memory::aligned_vector<lane_t<Num>>& results, const size_t padded_paths) {
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
                frontend::Program<Num> b_prime_prog = frontend::compile(b_prime_ast);
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
                frontend::Program<Num> b_prime_prog = frontend::compile(b_prime_ast);
                engine::CPU::BytecodeEval<Num> b_prime_funct(b_prime_prog);
                engine::CPU::milstein_functor milstein_funct;
                engine::CPU::dispatch_simulation<Num, engine::CPU::BytecodeEval<Num>, engine::CPU::milstein_functor>(config, results.data(), a_funct, b_funct, b_prime_funct, milstein_funct, padded_paths);
            }
        }
    }

    template<concepts::fp_or_simd Num>
    void typed_AST_dispatch(Config& config, memory::aligned_vector<lane_t<Num>>& results, size_t padded_paths) {
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
                engine::CPU::ASTSafeEval<Num> b_prime_funct(b_prime_ast);
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
                engine::CPU::ASTEval<Num> b_prime_funct(b_prime_ast);
                engine::CPU::milstein_functor milstein_funct;
                engine::CPU::dispatch_simulation<Num, engine::CPU::ASTEval<Num>, engine::CPU::milstein_functor>(config, results.data(), a_funct, b_funct, b_prime_funct, milstein_funct, padded_paths);
            }
        }
    }
}

template<concepts::FloatingPoint Num>
memory::aligned_vector<Num> sde::bytecode_dispatch(Config& config) {
    memory::aligned_vector<Num> results;
    if (config.use_simd) {
        size_t simd_width = stdx::native_simd<Num>::size();
        size_t num_paths = config.num_paths;
        size_t padded_paths = ((num_paths + simd_width - 1) / simd_width) * simd_width;
        results.resize(padded_paths * config.num_steps);
        typed_bytecode_dispatch<stdx::native_simd<Num>>(config, results, padded_paths);
    }else {
        results.resize(config.num_steps * config.num_paths);
        typed_bytecode_dispatch<Num>(config, results, config.num_paths);
    }
    return results;
}

template<concepts::FloatingPoint Num>
memory::aligned_vector<Num> sde::AST_dispatch(Config& config) {
    memory::aligned_vector<Num> results;
    if (config.use_simd) {
        size_t simd_width = stdx::native_simd<Num>::size();
        size_t num_paths = config.num_paths;
        size_t padded_paths = ((num_paths + simd_width - 1) / simd_width) * simd_width;
        results.resize(padded_paths * config.num_steps);
        typed_AST_dispatch<stdx::native_simd<Num>>(config, results, padded_paths);
    }else {
        results.resize(config.num_steps * config.num_paths);
        typed_AST_dispatch<Num>(config, results, config.num_paths);
    }
    return results;
}

template memory::aligned_vector<double>
sde::bytecode_dispatch<double>(sde::Config&);

template memory::aligned_vector<float>
sde::bytecode_dispatch<float>(sde::Config&);

template memory::aligned_vector<double>
sde::AST_dispatch<double>(sde::Config&);

template memory::aligned_vector<float>
sde::AST_dispatch<float>(sde::Config&);