#pragma once

#include <cstddef>

#ifdef _MSC_VER
#define SIMD_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define SIMD_INLINE inline __attribute__((always_inline))
#else
#define SIMD_INLINE inline
#endif

#ifdef __AVX512F__
#define SDE_HAS_AVX512 1
#else
#define SDE_HAS_AVX512 0
#endif

#ifdef __AVX__
#define SDE_HAS_AVX 1
#else
#define SDE_HAS_AVX 0
#endif


#ifdef __ARM_NEON
#define SDE_HAS_NEON 1
#else
#define SDE_HAS_NEON 0
#endif

#if SDE_HAS_AVX512
#include <immintrin.h>
using float_lane = __m512;
using double_lane = __m512d;
using float_mask = __mmask16;
using double_mask = __mmask8;
#define SDE_FLOAT_WIDTH 16
#define SDE_DOUBLE_WIDTH 8
#elif SDE_HAS_AVX
#include <immintrin.h>
using float_lane = __m256;
using double_lane = __m256d;
using float_mask = __m256;
using double_mask = __m256d;
#define SDE_FLOAT_WIDTH 8
#define SDE_DOUBLE_WIDTH 4
#elif SDE_HAS_NEON
#include <arm_neon.h>
using float_lane = float32x4_t;
using double_lane = float64x2_t;
using float_mask = uint32x4_t;
using double_mask = uint64x2_t;
#define SDE_FLOAT_WIDTH 4
#define SDE_DOUBLE_WIDTH 2
#else
#warning "no SIMD detected"
using float_lane = float;
using double_lane = double;
using float_mask = bool;
using double_mask = bool;
#define SDE_FLOAT_WIDTH 1
#define SDE_DOUBLE_WIDTH 1
#endif


namespace sde::simd {

    template<class T>
    concept simd_fp = std::is_same_v<T, float> || std::is_same_v<T, double>;

    template<simd_fp Num>
    struct simd_mask;

    template<>
    struct simd_mask<float> {
        float_mask v;
        simd_mask() = default;
        simd_mask(float_mask v) : v(v) {}
    };

    template<>
    struct simd_mask<double> {
        double_mask v;
        simd_mask() = default;
        simd_mask(double_mask v) : v(v) {}
    };
    template<simd_fp Num>
    struct simd;

    template<>
    struct simd<float> {
        float_lane v;
        static constexpr size_t size = SDE_FLOAT_WIDTH;
        static constexpr size_t width = SDE_FLOAT_WIDTH;
        using value_type = float;

        constexpr SIMD_INLINE simd() = default;

        template<class U>
        requires std::is_arithmetic_v<U> && (!std::is_same_v<U, bool>)
        constexpr SIMD_INLINE simd(U value) : simd(static_cast<float>(value)) {}


        explicit SIMD_INLINE simd(float x);

#if SDE_FLOAT_WIDTH > 1
        explicit SIMD_INLINE simd(float_lane v) : v(v) {}
#endif

        static SIMD_INLINE simd load(const float* ptr);
        SIMD_INLINE void store(float* ptr) const;

        SIMD_INLINE simd operator+(simd other) const;
        SIMD_INLINE simd operator-(simd other) const;
        SIMD_INLINE simd operator*(simd other) const;
        SIMD_INLINE simd operator/(simd other) const;
        SIMD_INLINE simd operator-() const;

        SIMD_INLINE simd& operator+=(simd other);
        SIMD_INLINE simd& operator-=(simd other);
        SIMD_INLINE simd& operator*=(simd other);
        SIMD_INLINE simd& operator/=(simd other);

        SIMD_INLINE float operator[](size_t i) const;
    };

    template<>
    struct simd<double> {
        double_lane v;
        static constexpr size_t size = SDE_DOUBLE_WIDTH;
        static constexpr size_t width = SDE_DOUBLE_WIDTH;
        using value_type = double;

        constexpr SIMD_INLINE simd() = default;

        template<class U>
        requires std::is_arithmetic_v<U> && (!std::is_same_v<U, bool>)
        constexpr SIMD_INLINE simd(U value) : simd(static_cast<double>(value)) {}

        explicit SIMD_INLINE simd(double x);
#if SDE_DOUBLE_WIDTH > 1
        explicit SIMD_INLINE simd(double_lane v) : v(v) {}
#endif

        static SIMD_INLINE simd load(const double* ptr);
        SIMD_INLINE void store(double* ptr) const;

        SIMD_INLINE simd operator+(simd other) const;
        SIMD_INLINE simd operator-(simd other) const;
        SIMD_INLINE simd operator*(simd other) const;
        SIMD_INLINE simd operator/(simd other) const;
        SIMD_INLINE simd operator-() const;

        SIMD_INLINE simd& operator+=(simd other);
        SIMD_INLINE simd& operator-=(simd other);
        SIMD_INLINE simd& operator*=(simd other);
        SIMD_INLINE simd& operator/=(simd other);

        SIMD_INLINE double operator[](size_t i) const;


    };
}


#if SDE_HAS_AVX512
#include "types-AVX512-impl.hpp"
#elif SDE_HAS_AVX
#include "types-AVX-impl.hpp"
#elif SDE_HAS_NEON
#include "types-NEON-impl.hpp"
#else
#include "types-scalar-impl.hpp"
#endif