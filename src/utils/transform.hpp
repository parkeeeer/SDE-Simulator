#pragma once

#include <cmath>
#include "rng.hpp"
#include <numbers>



#ifndef SDE_HAS_BUILTIN
    #if defined(__has_builtin)
        #define SDE_HAS_BUILTIN(x) __has_builtin(x)
    #else
        #define SDE_HAS_BUILTIN(x) 0
    #endif
#endif





namespace sde::rng {

    //you need one of these per thread
    template<sde::concepts::FloatingPoint Num>
    struct BMstate{
        bool has_spare = false;
        Num spare = 0;
    };
    

    inline void sincos(double x, double& sinx, double& cosx){
        #if defined(__GNUC__) && !defined(__clang__)
            __builtin_sincos(x, &sinx, &cosx);
        #elif SDE_HAS_BUILTIN(__builtin_sincos)
            __builtin_sincos(x, &sinx, &cosx);
        #else
            sinx = std::sin(x);
            cosx = std::cos(x);
        #endif
    }

    inline void sincos(float x, float& sinx, float& cosx){
        #if defined(__GNUC__) && !defined(__clang__)
            __builtin_sincosf(x, &sinx, &cosx);
        #elif SDE_HAS_BUILTIN(__builtin_sincosf)
            __builtin_sincosf(x, &sinx, &cosx);
        #else
            sinx = std::sinf(x);
            cosx = std::cosf(x);
        #endif
    }


    //uses box-muller to generate
    template<class Num>
    inline Num normal01(sde::rng::BMstate<Num>& state, Num i, Num j) {
        if(state.has_spare){
            state.has_spare = false;
            return state.spare;
        }

        const Num r = std::sqrt(-2.0 * std::log(i));
        const Num theta = 2.0 * static_cast<Num>(std::numbers::pi) * j;
        
        Num sinth, costh;
        sincos(theta, sinth, costh);
        state.spare = r * costh;
        state.has_spare = true;
        return r * sinth;
    }

}


