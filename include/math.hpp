#pragma once

#include "concepts.hpp"
#include "minisimd.hpp"
#include <cmath>


namespace sde::math {

    template<concepts::fp_or_simd Num>
    constexpr Num simd_cast(double val) {
        if constexpr(std::is_same_v<Num, double>) {
            return val;
        }else if constexpr(std::is_same_v<Num, simd::doublev>) {
            return static_cast<Num>(val);
        }else if constexpr(std::is_same_v<Num, float>) {
            return static_cast<Num>(val);
        }else if constexpr(std::is_same_v<Num, simd::floatv>) {
            return static_cast<Num>(static_cast<float>(val));
        }
    }

    template<concepts::fp_or_simd Num>
    constexpr concepts::lane_t<Num> scalar_extract(const Num& val) {
        if constexpr(concepts::is_native_simd<Num>::value) {
            return val[0];
        } else {
            return val;
        }
    }

    //returns epsilon gor the given type(smallest increment for said type)
    template<concepts::fp_or_simd Num>
    consteval Num epsilon(){
        if constexpr (std::is_same_v<Num,float>) {
            return 1e-6f;
        } else if constexpr (std::is_same_v<Num,double>) {
            return 1e-12;
        }
    }

    //-----------functions below are just to use the correct version of functions(scalar or simd version)--------------//

    template<concepts::fp_or_simd Num>
    Num sqrt(Num x) {
        using std::sqrt;
        using simd::sqrt;
        return sqrt(x);
    }

    template<concepts::fp_or_simd Num>
    Num log(Num x) {
        using std::log;
        using simd::log;
        return log(x);
    }

    template<concepts::fp_or_simd Num>
    Num abs(Num x) {
        using std::abs;
        using simd::abs;
        return abs(x);
    }

    template<concepts::fp_or_simd Num>
    Num sin(Num x) {
        using std::sin;
        using simd::sin;
        return sin(x);
    }

    template<concepts::fp_or_simd Num>
    Num cos(Num x) {
        using std::cos;
        using simd::cos;
        return cos(x);
    }

    template<concepts::fp_or_simd Num>
    Num tan(Num x) {
        using std::tan;
        using simd::tan;
        return tan(x);
    }

    template<concepts::fp_or_simd Num>
    Num sinh(Num x) {
        using std::sinh;
        using simd::sinh;
        return sinh(x);
    }

    template<concepts::fp_or_simd Num>
    Num cosh(Num x) {
        using std::cosh;
        using simd::cosh;
        return cosh(x);
    }

    template<concepts::fp_or_simd Num>
    Num tanh(Num x) {
        using std::tanh;
        using simd::tanh;
        return tanh(x);
    }

    template<concepts::fp_or_simd Num>
    Num exp(Num x) {
        using std::exp;
        using simd::exp;
        return exp(x);
    }

    template<concepts::fp_or_simd Num>
    Num min(Num a, Num b) {
        using std::min;
        using simd::min;
        return min(a,b);
    }

    template<concepts::fp_or_simd Num>
    Num max(Num a, Num b) {
        using std::max;
        using simd::max;
        return max(a,b);
    }

    template<concepts::fp_or_simd Num>
    Num pow(Num a, Num b) {
        using std::pow;
        using simd::pow;
        return pow(a,b);
    }

    //clamps logarithm to epsilon to make sure domain works
    //would love to make constexpr but that would be a lie because the standard doesnt have
    //their math functions as constexpr
    template<concepts::fp_or_simd Num>
    inline Num safe_log(Num input){
        return log(max(input, Num{epsilon<concepts::lane_t<Num>>()}));
    }

    //clamps the input to zero to avoid problems with domain
    template<concepts::fp_or_simd Num>
    inline Num safe_sqrt(Num input){
        return sqrt(max(input, static_cast<Num>(0)));
    }

    //clamps input to make sure domain works
    template<concepts::fp_or_simd Num>
    inline Num safe_div(Num numerator, Num denominator){
        if constexpr (concepts::is_native_simd<Num>::value) {
            const Num eps = Num(epsilon<concepts::lane_t<Num>>());
            Num abs_denom = math::abs(denominator);
            Num safe_abs = max(abs_denom, eps);
            Num sign = denominator / abs_denom; //+-1
            Num safe_denom = safe_abs * sign;
            return numerator / safe_denom;
        }else {
            if (abs(denominator) <= Num{epsilon<Num>()}) {
                if(denominator >= static_cast<Num>(0)) {
                    return numerator / Num{epsilon<Num>()};
                } else {
                    return numerator / -Num{epsilon<Num>()};
                }
            }
            return numerator / denominator;
        }
    }

    //approximation for the max function
    //replaces max when differentiating because this is differentiable everywhere
    //takes the max first for numerical stability
    template<concepts::fp_or_simd Num>
    Num lse_max(Num a, Num b, Num k = simd_cast<Num>(10.0)) {
        Num m = max(a,b);
        return m + (exp(k * (a - m)) + exp(k * (b - m))) / k;
    }

    //approximation for the min function
    //replaces min when differentiating because this is differentiable everywhere
    //takes the min first to retain numerical stability
    template<concepts::fp_or_simd Num>
    Num lse_min(Num a, Num b, Num k = simd_cast<Num>(10.0)) {
        Num m = min(a,b);
        return m - log(exp(k*(m-a)) + exp(k*(m-b))) / k;
    }

    //returns the weight of a in the softmax
    //for min function this will actually give the weight of b
    //useful to take derivative of lse
    //sigmoid to avoid overflow
    template<concepts::fp_or_simd Num>
    Num softmax_weight(Num a, Num b, Num k = simd_cast<Num>(10.0)) {
        return simd_cast<Num>(1.0) / (simd_cast<Num>(1.0) + exp(k * (b - a)));
    }


}