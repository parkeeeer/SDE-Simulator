#pragma once



#include <frontend.hpp>
//#include <bytecode.hpp>
#include <AST.hpp>
#include <rng.hpp>
#include <utils.hpp>
#include <concepts.hpp>

#include <thread>

#if __has_include(<simd>)
  #include <simd>
  namespace stdx = std;
#elif __has_include(<experimental/simd>)
  #include <experimental/simd>
  namespace stdx = std::experimental;
#else
  #error "No std::simd available"
#endif


using sde::concepts::lane_t;

using sde::concepts::is_native_simd;

namespace sde::engine::CPU {

    template<sde::concepts::fp_or_simd Num>
    constexpr size_t get_lane_size() {
      if constexpr(is_native_simd<Num>::value) {
        return Num::size();
      } else {
        return 1;
      }
    }


    template<sde::concepts::fp_or_simd Num, sde::concepts::Evaluator<Num> Eval>
    void euler_maruyama_runner(lane_t<Num>* paths, const size_t first_path_num, const size_t path_end,
                                Eval a, Eval b, Eval b_prime,
                                sde::rng::Xoshiro256Plus rng, sde::rng::BMstate<lane_t<Num>> state,
                                const Num sqrt_dt, const Num dt, const size_t num_steps, const size_t num_paths, const Num initial_value, const size_t padded_paths) noexcept;

    template<sde::concepts::fp_or_simd Num, sde::concepts::Evaluator<Num> Eval>
    void milstein_runner(lane_t<Num>* paths, size_t first_path_num, size_t path_end, 
                                  Eval a, Eval b, Eval b_prime,
                                  sde::rng::Xoshiro256Plus rng, sde::rng::BMstate<lane_t<Num>> state,
                                  const Num sqrt_dt, const Num dt, const size_t num_steps, const size_t num_paths, const Num initial_value, const size_t padded_paths) noexcept;

    struct euler_functor{
        template<sde::concepts::fp_or_simd Num, sde::concepts::Evaluator<Num> Eval>
        void operator()(lane_t<Num>* paths, const size_t first_path_num, const size_t path_end,
                                Eval a, Eval b, Eval b_prime,
                                sde::rng::Xoshiro256Plus rng, sde::rng::BMstate<lane_t<Num>> state,
                                const Num sqrt_dt, const Num dt, const size_t num_steps, const size_t num_paths, const Num initial_value, const size_t padded_paths) const noexcept {
            euler_maruyama_runner(paths, first_path_num, path_end, a, b, b_prime, rng, state, sqrt_dt, dt, num_steps, num_paths, initial_value, padded_paths);
        }
    };

    struct milstein_functor{
        template<sde::concepts::fp_or_simd Num, sde::concepts::Evaluator<Num> Eval>
        void operator()(lane_t<Num>* paths, size_t first_path_num, size_t path_end,
                                Eval a, Eval b, Eval b_prime,
                                sde::rng::Xoshiro256Plus rng, sde::rng::BMstate<lane_t<Num>> state,
                                Num sqrt_dt, Num dt, size_t num_steps, size_t num_paths, Num initial_value, const size_t padded_paths) const noexcept {
            milstein_runner(paths, first_path_num, path_end, a, b, b_prime, rng, state, sqrt_dt, dt, num_steps, num_paths, initial_value, padded_paths);
        }
    };

    

}




//----------implementations---------------//


#include <type_traits>


template<sde::concepts::fp_or_simd Num>
constexpr bool is_simd(){
    return is_native_simd<Num>::value;
}

template<sde::concepts::fp_or_simd Num, sde::concepts::Evaluator<Num> Eval>
void sde::engine::CPU::euler_maruyama_runner(lane_t<Num>* paths, const size_t first_path_num, const size_t path_end,
                    Eval a, Eval b, Eval b_prime,
                    sde::rng::Xoshiro256Plus rng, sde::rng::BMstate<lane_t<Num>> state,
                    const Num sqrt_dt, const Num dt, const size_t num_steps, const size_t num_paths, const Num initial_value, const size_t padded_paths) noexcept {
  (void) b_prime;
  constexpr int lane_size = CPU::get_lane_size<Num>();
  Num lane = Num(initial_value);

  for (size_t path_idx = first_path_num; path_idx < path_end; path_idx += lane_size) {
    if constexpr (is_simd<Num>()) {
      lane.copy_to(&paths[path_idx], stdx::vector_aligned_tag{});
    } else {
      paths[path_idx] = lane;
    }
  }

  for (size_t step = 1; step < num_steps; ++step) {
    for (size_t path = first_path_num; path < path_end; path += lane_size) {
      size_t idx = step * padded_paths + path;
      Num dw = sde::rng::random_normal(sqrt_dt, rng, state);
      Num x;
      if constexpr (is_simd<Num>()) {
        x.copy_from(&paths[idx - padded_paths], stdx::vector_aligned_tag{});
      }else{
        x = paths[idx - num_paths];
      }
      Num t = Num(static_cast<lane_t<Num>>(step)) * dt;
      Num new_lane = x + a(x, t) * dt + b(x, t) * dw;
      if constexpr (is_simd<Num>()) {
        new_lane.copy_to(&paths[idx], stdx::vector_aligned_tag{});
      }else{
        paths[idx] = new_lane;
      }
    }
  }
}



template<sde::concepts::fp_or_simd Num, sde::concepts::Evaluator<Num> Eval>
void sde::engine::CPU::milstein_runner(lane_t<Num>* paths, const size_t first_path_num, const size_t path_end,
                                  Eval a, Eval b, Eval b_prime,
                                  sde::rng::Xoshiro256Plus rng, sde::rng::BMstate<lane_t<Num>> state,
                                  const Num sqrt_dt, const Num dt, const size_t num_steps, const size_t num_paths, const Num initial_value, const size_t padded_paths) noexcept{
  constexpr int lane_size = get_lane_size<Num>();
  Num lane = Num(initial_value);
  for (size_t path_idx = first_path_num; path_idx < path_end; path_idx += lane_size) {
    if constexpr (is_simd<Num>()) {
      lane.copy_to(&paths[path_idx], stdx::vector_aligned_tag{});
    } else {
      paths[path_idx] = lane;
    }
  }
  for(size_t step = 1;step < num_steps;++step){
    for(size_t path = first_path_num;path < path_end;path += lane_size){
      size_t idx = step * padded_paths + path;
      Num dw = sde::rng::random_normal(sqrt_dt, rng, state);
      Num x;
      if constexpr (is_simd<Num>()){
        x.copy_from(&paths[idx - padded_paths], stdx::vector_aligned_tag{});
      }else{
        x = paths[idx - num_paths];
      }
      Num t = Num(static_cast<lane_t<Num>>(step)) * dt;
      Num b_lane = b(x,t);
      Num new_lane = x + a(x,t) * dt + b_lane * dw + Num(static_cast<lane_t<Num>>(.5)) * b_lane * b_prime(x,t) * (dw * dw - dt);
      if constexpr (is_simd<Num>()){
        new_lane.copy_to(&paths[idx], stdx::vector_aligned_tag{});
      }else{
        paths[idx] = new_lane;
      }
    }
  }
  std::cout << path_end << '\n';
}

