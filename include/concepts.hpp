#pragma once

#include <concepts>


//#include <type_traits>

#if __has_include(<simd>)
  #include <simd>
  namespace stdx = std;
#elif __has_include(<experimental/simd>)
  #include <experimental/simd>
  namespace stdx = std::experimental;
#else
  #error "No std::simd available"
#endif

namespace sde::concepts {

    template<class T>
    concept FloatingPoint = std::same_as<T, float> || std::same_as<T, double>;

    template<class T>
    concept fp_simd = std::same_as<T, stdx::native_simd<float>> || std::same_as<T, stdx::native_simd<double>>;

    template<class T>
    concept fp_or_simd = std::same_as<T, float> || std::same_as<T, double> || std::same_as<T, stdx::native_simd<float>> || std::same_as<T, stdx::native_simd<double>>;

    template<class Eval, class Num>
    concept Evaluator = requires(Eval e, Num x, Num t) {
        { e(x, t) } -> std::same_as<Num>;
    };




    //create a type which is either Num if a scalar or the lane type if a simd type
    template<class T>
    struct is_native_simd : std::false_type {};
    template<class T>
    struct is_native_simd<stdx::native_simd<T>> : std::true_type {};

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