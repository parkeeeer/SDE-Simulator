#pragma once

#include <thread>
#include <cmath>
#include <vector>

#include <utils.hpp>
#include <concepts.hpp>
#include <dispatch.hpp>
#include "CPUengine.hpp"
#include "math.hpp"

#if __has_include(<simd>)
  #include <simd>
  namespace stdx = std;
#elif __has_include(<experimental/simd>)
  #include <experimental/simd>
  namespace stdx = std::experimental;
#else
  #error "No std::simd available"
#endif


namespace sde::engine::CPU {

template<class Num>
struct BytecodeEval{
    const sde::frontend::Program<Num>& program;
    memory::aligned_vector<Num> stack;

    explicit BytecodeEval(const sde::frontend::Program<Num>& program) : program{program} {
        stack.reserve(program.get_max_stack_size());
    }

    //preserve capacity
    BytecodeEval(const BytecodeEval& other) : program{other.program} {
        stack.reserve(other.stack.capacity());
    }

    Num operator()(Num x, Num t) noexcept{
        return program.run(x, t, stack.data());
    }
};

template<class Num>
struct ASTEval{
    const sde::frontend::AST<Num>& ast;
    Num operator()(Num x, Num t) const noexcept{
        return ast.eval(x, t);
    }
};

template<class Num>
struct BytecodeSafeEval {
    const sde::frontend::Program<Num>& program;
    memory::aligned_vector<Num> stack;

    explicit BytecodeSafeEval(const sde::frontend::Program<Num>& program) : program{program} {
        stack.reserve(program.get_max_stack_size());
    }

    //preserve capacity
    BytecodeSafeEval(const BytecodeSafeEval& other) : program{other.program} {
        stack.reserve(other.stack.capacity());
    }
    Num operator()(Num x, Num t) noexcept{
        return program.safe_run(x, t, stack.data());
    }
};

template<class Num>
struct ASTSafeEval {
    const sde::frontend::AST<Num>& ast;
    Num operator()(Num X, Num t) const noexcept {
        return ast.safe_eval(X, t);
    }
};

template<class Num, sde::concepts::Evaluator<Num> Eval, class method>
void dispatch_simulation(sde::Config& options, lane_t<Num>* paths, const Eval& a, const Eval& b, const Eval& b_prime, const method& meth, const size_t padded_paths){

    std::cout << "simd size: " << get_lane_size<Num>() << '\n';
    const Num dt = static_cast<Num>(static_cast<lane_t<Num>>(options.dt));
    const Num sqrt_dt = math::sqrt(dt);
    const Num initial_value = static_cast<Num>(static_cast<lane_t<Num>>(options.initial_value));
    const size_t lane_size = CPU::get_lane_size<Num>();
    
    const size_t paths_per_thread = ((options.num_paths / options.threads) / lane_size) * lane_size;

    const size_t max_useful_threads = padded_paths / paths_per_thread;
    const size_t num_threads = std::min(options.threads, max_useful_threads);



    if(options.threads <= 1){
        auto rng = rng::make_prng(options.seed);
        auto bm_state = rng::BMstate<lane_t<Num>>{};

        meth(paths, 0, options.num_paths, a, b, b_prime, rng, bm_state, sqrt_dt, dt, options.num_steps, options.num_paths, initial_value, padded_paths);
        return;
    }
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    rng::BMstate<lane_t<Num>> base_state;
    rng::Xoshiro256Plus rng = rng::make_prng(options.seed);
    const size_t num_paths = options.num_paths;
    const size_t num_steps = options.num_steps;
    for(size_t i = 0; i < num_threads;i++){
        const size_t start = i * paths_per_thread;
        const size_t end = (i+1) * paths_per_thread;
        //std::cout << "thread " << i << " paths: " << start<< " to " << end - 1 << std::endl;

        auto paths_copy = paths;

        threads.emplace_back([paths_copy, a, b, b_prime, &meth, sqrt_dt, dt, num_steps, num_paths, initial_value,padded_paths, i,start,end, base_state, rng]{

            meth(paths_copy, start, end, a, b, b_prime, rng, base_state
                                            ,sqrt_dt, dt, num_steps, num_paths, initial_value, padded_paths);
            });
        rng.jump();
    }
    if (num_threads * paths_per_thread < padded_paths) {
        //std::cout << "final thread, paths : " << num_threads * paths_per_thread << " to " << padded_paths - 1 << std::endl;
        meth(paths, num_threads * paths_per_thread, padded_paths, a, b, b_prime, rng, base_state, sqrt_dt, dt, options.num_steps, options.num_paths, initial_value, padded_paths);
    }
    std::cout << "joining threads\n";
    for (size_t i = 0; i < num_threads; i++) {
        threads[i].join();
        std::cout << "thread " << i << " thread stopped\n";
    }
    std::cout << "leaving simulator\n";
}

}