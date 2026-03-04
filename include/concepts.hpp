#pragma once

#include <concepts>

#include "minisimd.hpp"


//#include <type_traits>




namespace sde::concepts {

    template<class T>
    concept FloatingPoint = std::same_as<T, float> || std::same_as<T, double>;

    template<class T>
    concept fp_simd = std::same_as<T, simd::floatv> || std::same_as<T, simd::doublev>;

    template<class T>
    concept fp_or_simd = std::same_as<T, float> || std::same_as<T, double> || std::same_as<T, simd::floatv> || std::same_as<T, simd::doublev>;

    template<class Eval, class Num>
    concept Evaluator = requires(Eval e, Num x, Num t) {
        { e(x, t) } -> std::same_as<Num>;
    };




    //create a type which is either Num if a scalar or the lane type if a simd type
    template<class T>
    struct is_native_simd : std::false_type {};
    template<class T>
    struct is_native_simd<simd::simd<T>> : std::true_type {};

    template<class T, bool is_simd = is_native_simd<T>::value>
    struct lane_t_impl {
        using type = T;
    };

    template<class T>
    struct lane_t_impl<T, true> {
        using type = typename T::value_type;
    };

    //type which will either be the scalar if scalar or the scalar value in a vector simd type
    template<class T>
    using lane_t = typename lane_t_impl<T>::type;
}