#pragma once

#include "rng.hpp"
#include "concepts.hpp"
#include "transform.hpp"
#include <iostream>




#if __has_include(<simd>)
  #include <simd>
  namespace stdx = std;
#elif __has_include(<experimental/simd>)
  #include <experimental/simd>
  namespace stdx = std::experimental;
#else
  #error "No std::simd available"
#endif

#include <concepts>
#include <type_traits>


namespace sde::rng {

    inline Xoshiro256Plus make_prng(uint64_t seed){
        return Xoshiro256Plus(seed);
    }

    
    template<sde::concepts::FloatingPoint Num>
    Num random_uniform(Xoshiro256Plus& rng){
        if constexpr (std::is_same_v<Num, float>){
            return u64_to_float(rng.next_u64());
        } else if constexpr (std::is_same_v<Num, double>){
            return u64_to_double(rng.next_u64());
        }
    }

    //uses Polar transform to go from uniform to normal distribution
    template<sde::concepts::fp_or_simd Num>
    Num random_normal(Num stddev, Xoshiro256Plus& rng, BMstate<concepts::lane_t<Num>>& state){
        if constexpr(concepts::is_native_simd<Num>::value) {
            Num result = Num();
            for(size_t i = 0; i < Num::size(); i++){
                concepts::lane_t<Num> ref = stddev[i];
                result[i] = random_normal(ref, rng, state);
            }
            return result;
        } else {
            Num u = random_uniform<Num>(rng);
            Num v = random_uniform<Num>(rng);
            return normal01(state, u, v) * stddev;
        }
    }
}


namespace sde::domain_error {
    enum class DomainErrorHandling{
        ABORT,
        WARN_AND_CONTINUE,
        IGNORE
    };

    inline void handle_bad_domain(DomainErrorHandling current_domain_error_handling){
        switch(current_domain_error_handling){
            case DomainErrorHandling::ABORT:
                throw std::runtime_error("Domain error encountered during parsing or evaluation");
            case DomainErrorHandling::WARN_AND_CONTINUE:
                std::cerr << "Warning: Domain error encountered during parsing or evaluation"
                 << std::endl << "will clamp to nearest valid value and continue" << std::endl
                 << "procede? (y/n): " << std::endl;
                char resp;
                std::cin >> resp;
                if(resp != 'y' && resp != 'Y'){
                    throw std::runtime_error("Aborting due to domain error");
                }
                break;
            case DomainErrorHandling::IGNORE:
                break;
        }
    }
}