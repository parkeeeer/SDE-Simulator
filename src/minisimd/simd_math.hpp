#pragma once

#include "types.hpp"
#include "helpers.hpp"
#include <cmath>


namespace sde::simd {
        template<simd_fp Num>
    SIMD_INLINE simd<Num> fma(simd<Num> a, simd<Num> b, simd<Num> c);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> sqrt(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> abs(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> exp(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> pow(simd<Num> a, simd<Num> b);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> log(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> sin(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> cos(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> tan(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> max(simd<Num> a, simd<Num> b);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> min(simd<Num> a, simd<Num> b);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> cosh(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> sinh(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> tanh(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> floor(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> ceil(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> round(simd<Num> x);

    //maybe
    template<simd_fp Num>
    SIMD_INLINE simd<Num> atan(simd<Num> x);
}

namespace sde::simd::detail {

    template<simd_fp Num>
    struct trig_reduction {
        simd<Num> reduced;
        simd<Num> quadrant;
    };

    template<simd_fp Num>
    SIMD_INLINE trig_reduction<Num> cody_waite(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> sin_eval(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> cos_eval(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> exp_eval(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> log_eval(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> pow2(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> scalebn(simd<Num> x, simd<Num> n);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> get_mantissa(simd<Num> x);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> get_exponent(simd<Num> x);
}



//==========================implementations==========================//


namespace sde::simd {
#if SDE_HAS_AVX512
    template<>
    inline simd<float> fma(simd<float> a, simd<float> b, simd<float> c) {
        return simd<float>(_mm512_fmadd_ps(a.v, b.v, c.v));
    }

    template<>
    inline simd<double> fma(simd<double> a, simd<double> b, simd<double> c) {
        return simd<double>(_mm512_fmadd_pd(a.v, b.v, c.v));
    }
#elif SDE_HAS_AVX
    template<>
    inline simd<float> fma(simd<float> a, simd<float> b, simd<float> c) {
        return simd<float>(_mm256_fmadd_ps(a.v,b.v,c.v));
    }

    template<>
    inline simd<double> fma(simd<double> a, simd<double> b, simd<double> c) {
        return simd<double>(_mm256_fmadd_pd(a.v,b.v,c.v));
    }

    template<>
    inline simd<float> sqrt(simd<float> x) {
        return simd<float>(_mm256_sqrt_ps(x.v));
    }

    template<>
    inline simd<double> sqrt(simd<double> x) {
        return simd<double>(_mm256_sqrt_pd(x.v));
    }

    template<>
    inline simd<float> round(simd<float> x) {
        return simd<float>{_mm256_round_ps(x.v, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC)};
    }

    template<>
    inline simd<double> round(simd<double> x) {
        return simd<double>{_mm256_round_pd(x.v, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC)};
    }

    template<>
    inline simd<float> abs(simd<float> x) {
        __m256 sign_mask = _mm256_set1_ps(-1.0f);
        return simd<float>{_mm256_andnot_ps(sign_mask, x.v)};
    }

    template<>
    inline simd<double> abs(simd<double> x) {
        __m256d sign_mask = _mm256_set1_pd(-1.0);
        return simd<double>{_mm256_andnot_pd(sign_mask, x.v)};
    }

    template<>
    inline simd<float> max(simd<float> a, simd<float> b) {
        return simd<float>(_mm256_max_ps(a.v, b.v));
    }

    template<>
    inline simd<double> max(simd<double> a, simd<double> b) {
        return simd<double>(_mm256_max_pd(a.v, b.v));
    }

    template<>
    inline simd<float> min(simd<float> a, simd<float> b) {
        return simd<float>(_mm256_min_ps(a.v, b.v));
    }

    template<>
    inline simd<double> min(simd<double> a, simd<double> b) {
        return simd<double>(_mm256_min_pd(a.v, b.v));
    }

    template<>
    inline simd<float> detail::pow2(simd<float> x) {
        __m256i x_int = _mm256_cvtps_epi32(x.v);
        __m256i bias = _mm256_add_epi32(x_int, _mm256_set1_epi32(127));
        __m256i exponent = _mm256_slli_epi32(bias, 23);

        return simd<float>{_mm256_castsi256_ps(exponent)};
    }

    template<>
    inline simd<double> detail::pow2(simd<double> x) {
        __m128d lo_pd = _mm256_castpd256_pd128(x.v);
        __m128d hi_pd = _mm256_extractf128_pd(x.v, 1);

        __m128i lo_i32 = _mm_cvttpd_epi32(lo_pd);
        __m128i hi_i32 = _mm_cvttpd_epi32(hi_pd);

        __m128i all_i32 = _mm_unpacklo_epi64(lo_i32, hi_i32);

        __m256i i64 = _mm256_cvtepi32_epi64(all_i32);

        __m256i bias = _mm256_add_epi64(i64, _mm256_set1_epi64x(1023));
        __m256i exponent = _mm256_slli_epi64(bias, 52);

        return simd<double>{_mm256_castsi256_pd(exponent)};
    }

    template<>
    inline simd<float> floor(simd<float> a) {
        return simd<float>{_mm256_floor_ps(a.v)};
    }

    template<>
    inline simd<double> floor(simd<double> a) {
        return simd<double>{_mm256_floor_pd(a.v)};
    }

    template<>
    inline simd<float> ceil(simd<float> a) {
        return simd<float>{_mm256_ceil_ps(a.v)};
    }

    template<>
    inline simd<double> ceil(simd<double> a) {
        return simd<double>{_mm256_ceil_pd(a.v)};
    }

    template<>
inline simd<float> detail::get_mantissa(simd<float> x) {
        __m256i to_int = _mm256_castps_si256(x.v);
        __m256i mantissa_mask = _mm256_set1_epi32(0x807FFFFF);
        __m256i exp_zero = _mm256_set1_epi32(127 << 23);
        __m256i mantissa = _mm256_and_si256(to_int, mantissa_mask);
        __m256i result = _mm256_or_si256(exp_zero, mantissa);
        return simd<float>{_mm256_castsi256_ps(result)};
    }

    template<>
    inline simd<double> detail::get_mantissa(simd<double> x) {
        __m256i to_int = _mm256_castpd_si256(x.v);
        __m256i mantissa_mask = _mm256_set1_epi64x(0x800FFFFFFFFFFFFFULL);
        __m256i exp_zero = _mm256_set1_epi64x(1023ULL << 52);
        __m256i mantissa = _mm256_and_si256(to_int, mantissa_mask);
        __m256i result = _mm256_or_si256(exp_zero, mantissa);
        return simd<double>{_mm256_castsi256_pd(result)};
    }

    template<>
    inline simd<float> detail::get_exponent(simd<float> x) {
        __m256i to_int = _mm256_castps_si256(x.v);
        __m256i exp_bits = _mm256_srli_epi32(to_int, 23);
        __m256i exp_mask = _mm256_set1_epi32(0xFF);
        __m256i biased_exp = _mm256_and_si256(exp_bits, exp_mask);
        __m256i unbiased = _mm256_sub_epi32(biased_exp, _mm256_set1_epi32(127));
        return simd<float>{_mm256_cvtepi32_ps(unbiased)};
    }

    template<>
    inline simd<double> detail::get_exponent(simd<double> x) {
        __m256i to_int = _mm256_castpd_si256(x.v);
        __m256i exp_bits = _mm256_srli_epi64(to_int, 52);
        __m256i exp_mask = _mm256_set1_epi64x(0x7FF);
        __m256i biased_exp = _mm256_and_si256(exp_bits, exp_mask);
        __m256i unbiased = _mm256_sub_epi64(biased_exp, _mm256_set1_epi64x(1023));
        __m128i lo = _mm256_castsi256_si128(unbiased);
        __m128i hi = _mm256_extracti128_si256(unbiased, 1);
        __m128d lo_d = _mm_cvtepi32_pd(lo);
        __m128d hi_d = _mm_cvtepi32_pd(hi);
        return simd<double>{_mm256_set_m128d(hi_d, lo_d)};
    }
#elif SDE_HAS_NEON
    template<>
    inline simd<float> fma(simd<float> a, simd<float> b, simd<float> c) {
        return simd<float>(vmlaq_f32(c.v, a.v, b.v));
    }

    template<>
    inline simd<double> fma(simd<double> a, simd<double> b, simd<double> c) {
        return simd<double>(vmlaq_f64(c.v, a.v, b.v));
    }

    template<>
    inline simd<float> round(simd<float> x) {
        return simd<float>(vrndnq_f32(x.v));
    }

    template<>
    inline simd<double> round(simd<double> x) {
        return simd<double>(vrndnq_f64(x.v));
    }

    template<>
    inline simd<float> sqrt(simd<float> x) {
        return simd<float>(vsqrtq_f32(x.v));
    }

    template<>
    inline simd<double> sqrt(simd<double> x) {
        return simd<double>(vsqrtq_f64(x.v));
    }

    template<>
    inline simd<float> abs(simd<float> x) {
        return simd<float>(vabsq_f32(x.v));
    }

    template<>
    inline simd<double> abs(simd<double> x) {
        return simd<double>(vabsq_f64(x.v));
    }

    template<>
    inline simd<float> max(simd<float> a, simd<float> b) {
        return simd<float>(vmaxq_f32(a.v,b.v));
    }

    template<>
    inline simd<double> max(simd<double> a, simd<double> b) {
        return simd<double>(vmaxq_f64(a.v,b.v));
    }

    template<>
    inline simd<float> min(simd<float> a, simd<float> b) {
        return simd<float>(vminq_f32(a.v,b.v));
    }

    template<>
    inline simd<double> min(simd<double> a, simd<double> b) {
        return simd<double>(vminq_f64(a.v,b.v));
    }

    template<>
    inline simd<float> detail::pow2(simd<float> x) {
        int32x4_t x_to_int = vcvtq_s32_f32(x.v);

        int32x4_t bias = vaddq_s32(x_to_int, vdupq_n_s32(127));
        int32x4_t exponent = vshlq_n_s32(bias, 23);

        return simd<float>{vreinterpretq_f32_s32(exponent)};
    }

    template<>
    inline simd<double> detail::pow2(simd<double> x) {
        int64x2_t x_to_int = vcvtq_s64_f64(x.v);

        int64x2_t bias = vaddq_s64(x_to_int, vdupq_n_s64(1023));
        int64x2_t exponent = vshlq_n_s64(bias, 52);

        return simd<double>{vreinterpretq_f64_s64(exponent)};
    }

    template<>
    inline simd<float> detail::scalebn(simd<float> x, simd<float> n) {
        return x * pow2(n);
    }

    template<>
    inline simd<double> detail::scalebn(simd<double> x, simd<double> n) {
        return x * pow2(n);
    }

    template<>
    inline simd<float> detail::get_mantissa(simd<float> x) {
        uint32x4_t to_int = vreinterpretq_u32_f32(x.v);
        uint32x4_t mantissa_mask = vdupq_n_u32(0x807FFFFF);
        uint32x4_t exp_zero = vdupq_n_u32(127 << 23);
        uint32x4_t mantissa = vandq_u32(to_int, mantissa_mask);
        uint32x4_t result = vorrq_u32(exp_zero, mantissa);
        return simd<float>{vreinterpretq_f32_u32(result)};
    }

    template<>
    inline simd<double> detail::get_mantissa(simd<double> x) {
        uint64x2_t to_int = vreinterpretq_u64_f64(x.v);
        uint64x2_t mantissa_mask = vdupq_n_u64(0x800FFFFFFFFFFFFFULL);
        uint64x2_t exp_zero = vdupq_n_u64(1023ULL << 52);
        uint64x2_t mantissa = vandq_u64(to_int, mantissa_mask);
        uint64x2_t result = vorrq_u64(exp_zero, mantissa);
        return simd<double>{vreinterpretq_f64_u64(result)};
    }

    template<>
    inline simd<float> detail::get_exponent(simd<float> x) {
        uint32x4_t to_int = vreinterpretq_u32_f32(x.v);
        uint32x4_t exp_bits = vshrq_n_u32(to_int, 23);
        uint32x4_t exp_mask = vdupq_n_u32(0xFF);
        uint32x4_t biased_exp = vandq_u32(exp_bits, exp_mask);
        int32x4_t signed_exp = vreinterpretq_s32_u32(biased_exp);
        int32x4_t unbiased = vsubq_s32(signed_exp, vdupq_n_s32(127));
        return simd<float>{vcvtq_f32_s32(unbiased)};
    }

    template<>
    inline simd<double> detail::get_exponent(simd<double> x) {
        uint64x2_t to_int = vreinterpretq_u64_f64(x.v);
        uint64x2_t exp_bits = vshrq_n_u64(to_int, 52);
        uint64x2_t exp_mask = vdupq_n_u64(0x7FF);
        uint64x2_t biased_exp = vandq_u64(exp_bits, exp_mask);
        int64x2_t signed_exp = vreinterpretq_s64_u64(biased_exp);
        int64x2_t unbiased = vsubq_s64(signed_exp, vdupq_n_s64(1023));
        return simd<double>{vcvtq_f64_s64(unbiased)};
    }

    template<>
    inline simd<float> floor(simd<float> x) {
        return simd<float>{vrndmq_f32(x.v)};
    }

    template<>
    inline simd<double> floor(simd<double> x) {
        return simd<double>{vrndmq_f64(x.v)};
    }

    template<>
    inline simd<float> ceil(simd<float> x) {
        return simd<float>{vrndpq_f32(x.v)};
    }

    template<>
    inline simd<double> ceil(simd<double> x) {
        return simd<double>{vrndpq_f64(x.v)};
    }
#else
    inline simd<float> fma(simd<float> a, simd<float> b, simd<float> c) {
        return std::fmaf(a.v,b.v,c.v);
    }

    inline simd<double> fma(simd<double> a, simd<double> b, simd<double> c) {
        return std::fma(a.v,b.v,c.v);
    }
#endif

    template<>
    inline detail::trig_reduction<float> detail::cody_waite(simd<float> x) {
        constexpr float two_over_pi = 0x1.45f306p-1f;
        constexpr float pio2_1 = 0x1.921fb0p+0f;
        constexpr float pio2_2 = 0x1.5110b4p-22f;
        constexpr float pio2_3 = 0x1.846988p-48f;

        simd<float> n = round(x * simd<float>(two_over_pi));

        simd<float> r = fma(n, simd<float>(-pio2_1), x);
        r = fma(n, simd<float>(pio2_2), r);
        r = fma(n, simd<float>(pio2_3), r);
        return {r, n};
    }

    template<>
    inline detail::trig_reduction<double> detail::cody_waite(simd<double> x) {
        constexpr double two_over_pi = 0x1.45f306dc9c883p-1;
        constexpr double pio2_1 = 0x1.921fb54442d18p+0;
        constexpr double pio2_2 = 0x1.1a62633145c07p-54;
        constexpr double pio2_3 = 0x1.c1cd129024e09p-108;
        constexpr double pio2_4 = 0x1.3198a2e037073p-162;

        simd<double> n = round(x * simd<double>(two_over_pi));

        simd<double> r = fma(n, simd<double>(-pio2_1), x);
        r = fma(n, simd<double>(pio2_2), r);
        r = fma(n, simd<double>(pio2_3), r);
        r = fma(n, simd<double>(pio2_4), r);
        return {r, n};
    }

    template<>
    inline simd<float> detail::sin_eval(simd<float> x) {
        simd<float> x2 = x * x;

        simd<float> p(0x1.7d3bbcp-19f);
        p = fma(p, x2, simd<float>(-0x1.a06bbap-13f));
        p = fma(p, x2, simd<float>(0x1.11119ap-07f));
        p = fma(p, x2, simd<float>(-0x1.555556p-03f));
        p = p * x2;
        return fma(p,x,x);
    }

    template<>
    inline simd<float> detail::cos_eval(simd<float> x) {
        simd<float> x2 = x * x;

        simd<float> p(0x1.98e616p-16f);
        p = fma(p, x2, simd<float>(-0x1.6c06dcp-10f));
        p = fma(p, x2, simd<float>(0x1.55553cp-05f));
        p = fma(p, x2, simd<float>(-0x1.000000p-01f));
        return fma(p, x2, simd<float>(1.0f));
    }

    template<>
    inline simd<double> detail::sin_eval(simd<double> x) {
        simd<double> x2 = x * x;

        simd<double> p(0x1.1ee9d7b4e3f05p-41);
        p = fma(p, x2, simd<double>(-0x1.93974a8c07c9dp-36));
        p = fma(p, x2, simd<double>(0x1.1ee9ebdb4b1c4p-30));
        p = fma(p, x2, simd<double>(-0x1.ae5e68a2b9cebp-25));
        p = fma(p, x2, simd<double>(0x1.71de3a556c734p-19));
        p = fma(p, x2, simd<double>(-0x1.a01a01a018b26p-13));
        p = fma(p, x2, simd<double>(0x1.1111111110f8ap-07));
        p = fma(p, x2, simd<double>(-0x1.5555555555549p-03));
        p = p * x2;
        return fma(p, x, x);
    }

    template<>
    inline simd<double> detail::cos_eval(simd<double> x) {
        simd<double> x2 = x * x;

        simd<double> p(0x1.8f76c8bd1f1a8p-26);
        p = fma(p, x2, simd<double>(-0x1.6c16c16c14f91p-20));
        p = fma(p, x2, simd<double>(0x1.55555555554e6p-14));
        p = fma(p, x2, simd<double>(-0x1.6c16c16c16967p-09));
        p = fma(p, x2, simd<double>(0x1.5555555555555p-05));
        p = fma(p, x2, simd<double>(-0x1.0000000000000p-01));
        return fma(p, x2, simd<double>(1.0));
    }

    template<>
    inline simd<float> sin(simd<float> x) {
        simd<float> abs_x = abs(x);
        simd<float> signs = x & simd<float>(-0.0f);
        auto [xr, n] = detail::cody_waite(abs_x);

        simd<float> nmod4 = n - simd<float>(4.0f) * floor(n * simd<float>(.25f));

        auto use_cos = ((nmod4 >= simd<float>(1.0f)) && (nmod4 < simd<float>(2.0f))) ||
               (nmod4 >= simd<float>(3.0f));

        auto flip_sign = nmod4 >= simd<float>(2.0f);

        simd<float> result_sign = signs ^ select(flip_sign, simd<float>(-0.0f), simd<float>(0.0f));

        simd<float> sin_result = detail::sin_eval(xr);
        simd<float> cos_result = detail::cos_eval(xr);

        simd<float> result = select(use_cos, cos_result, sin_result);
        return result ^ result_sign;
    }

    template<>
    inline simd<float> cos(simd<float> x) {
        simd<float> abs_x = abs(x);
        //cos is even so no sign logic yay!!
        auto [xr, n] = detail::cody_waite(abs_x);

        n = n + simd<float>(1.f);

        simd<float> nmod4 = n - simd<float>(4.0f) * floor(n * simd<float>(.25f));

        auto use_cos = ((nmod4 >= simd<float>(1.0f)) && (nmod4 < simd<float>(2.0f))) ||
               (nmod4 >= simd<float>(3.0f));

        auto flip_sign = nmod4 >= simd<float>(2.0f);
        simd<float> result_sign = select(flip_sign, simd<float>(-0.0f), simd<float>(0.0f));

        simd<float> sin_result = detail::sin_eval(xr);
        simd<float> cos_result = detail::cos_eval(xr);

        simd<float> result = select(use_cos, cos_result, sin_result);
        return result ^ result_sign;
    }

    template<>
    inline simd<double> sin(simd<double> x) {
        simd<double> abs_x = abs(x);
        simd<double> signs = x & simd<double>(-0.0);
        auto [xr, n] = detail::cody_waite(abs_x);

        simd<double> nmod4 = n - simd<double>(4.0) * floor(n * simd<double>(.25));

        auto use_cos = ((nmod4 >= simd<double>(1.0)) && (nmod4 < simd<double>(2.0))) ||
               (nmod4 >= simd<double>(3.0));

        auto flip_sign = nmod4 >= simd<double>(2.0);

        simd<double> result_sign = signs ^ select(flip_sign, simd<double>(-0.0), simd<double>(0.0));

        simd<double> sin_result = detail::sin_eval(xr);
        simd<double> cos_result = detail::cos_eval(xr);

        simd<double> result = select(use_cos,cos_result,sin_result);
        return result ^ result_sign;
    }

    template<>
    inline simd<double> cos(simd<double> x) {
        simd<double> abs_x = abs(x);
        //cos is even so no sign logic yay!!
        auto [xr, n] = detail::cody_waite(abs_x);

        n = n + simd<double>(1.0);
        simd<double> nmod4 = n - simd<double>(4.0) * floor(n * simd<double>(.25));

        auto use_cos = ((nmod4 >= simd<double>(1.0)) && (nmod4 < simd<double>(2.0))) ||
               (nmod4 >= simd<double>(3.0));

        auto flip_sign = nmod4 >= simd<double>(2.0);
        simd<double> result_sign = select(flip_sign, simd<double>(-0.0), simd<double>(0.0));

        simd<double> sin_result = detail::sin_eval(xr);
        simd<double> cos_result = detail::cos_eval(xr);

        simd<double> result = select(use_cos, cos_result, sin_result);
        return result ^ result_sign;
    }

    template<>
    inline simd<float> tan(simd<float> x) {
        simd<float> abs_x = abs(x);
        auto [xr, n] = detail::cody_waite(abs_x);

        simd<float> sin_result = detail::sin_eval(xr);
        simd<float> cos_result = detail::cos_eval(xr);

        simd<float> nmod2 = n - simd<float>(2.0f) * floor(n * simd<float>(0.5f));

        auto use_cot = nmod2 >= simd<float>(1.0f);
        return select(use_cot, -cos_result / sin_result, sin_result / cos_result);
    }

    template<>
    inline simd<double> tan(simd<double> x) {
        simd<double> abs_x = abs(x);
        auto [xr, n] = detail::cody_waite(abs_x);

        simd<double> sin_result = detail::sin_eval(xr);
        simd<double> cos_result = detail::cos_eval(xr);

        simd<double> nmod2 = n - simd<double>(2.0) * floor(n * simd<double>(0.5));

        auto use_cot = nmod2 >= simd<double>(1.0f);
        return select(use_cot, -cos_result / sin_result, sin_result / cos_result);
    }

    template<>
    inline simd<float> detail::exp_eval(simd<float> x) {
        simd<float> p(0x1.694000p-10f);
        p = fma(p, x, simd<float>(0x1.125edcp-07f));
        p = fma(p, x, simd<float>(0x1.555b5ap-05f));
        p = fma(p, x, simd<float>(0x1.555450p-03f));
        p = fma(p, x, simd<float>(0x1.fffff6p-02f));
        p = fma(p, x, simd<float>(1.0f));
        p = fma(p, x, simd<float>(1.0f));
        return p;
    }

    template<>
    inline simd<double> detail::exp_eval(simd<double> x) {
        // Minimax polynomial for exp(r) on [-0.35, 0.35]
        // Degree 10 from Sollya

        simd<double> p(2.5022322536e-8);
        p = fma(p, x, simd<double>(2.7629817929e-7));
        p = fma(p, x, simd<double>(2.7557508395e-6));
        p = fma(p, x, simd<double>(2.4801566879e-5));
        p = fma(p, x, simd<double>(1.9841269707e-4));
        p = fma(p, x, simd<double>(1.3888888934e-3));
        p = fma(p, x, simd<double>(8.3333333326e-3));
        p = fma(p, x, simd<double>(4.1666666666e-2));
        p = fma(p, x, simd<double>(0.166666666666));
        p = fma(p, x, simd<double>(0.5));
        p = fma(p, x, simd<double>(1.0));
        p = fma(p, x, simd<double>(1.0));

        return p;
    }

    template<>
    inline simd<float> exp(simd<float> x) {
        const simd<float> ln2_high(0x1.62e400p-1f);
        const simd<float> ln2_low(0x1.7f7d1cp-20f);
        const simd<float> inv_ln2(0x1.715476p+0f);
        const simd<float> max_input(88.0f);
        const simd<float> min_input(-87.0f);

        simd<float> clamped = max(min(x, max_input), min_input);

        simd<float> rounded = round(clamped * inv_ln2);
        simd<float> n = fma(rounded, -ln2_high, clamped);
        n = fma(rounded, -ln2_low, n);

        simd<float> result = detail::exp_eval(n);

        return result * detail::pow2(rounded);
    }

    inline simd<double> exp(simd<double> x) {
        const simd<double> ln2_high(0x1.62e42fefa39efp-1);
        const simd<double> ln2_low(0x1.71547652b82fep-20);
        const simd<double> inv_ln2(0x1.71547652b82fep+0);
        const simd<double> max_input(709.782712893384);
        const simd<double> min_input(-708.396418532208);

        simd<double> clamped = max(min(x, max_input), min_input);

        simd<double> rounded = round(clamped * inv_ln2);
        simd<double> n = fma(rounded, -ln2_high, clamped);
        n = fma(rounded, -ln2_low, n);

        simd<double> result = detail::exp_eval(n);

        return result * detail::pow2(rounded);
    }

    template<>
    inline simd<float> detail::log_eval(simd<float> x) {
        // minimax polynomial for log(x) on [0.5, 0.999999]
        // generated by sollya

        simd<float> p(0.9729421138763427734375f);
        p = fma(p, x, simd<float>(-6.2009067535400390625f));
        p = fma(p, x, simd<float>(17.2790775299072265625f));
        p = fma(p, x, simd<float>(-27.62737274169921875f));
        p = fma(p, x, simd<float>(28.022068023681640625f));
        p = fma(p, x, simd<float>(-19.0107860565185546875f));
        p = fma(p, x, simd<float>(9.46431732177734375f));
        p = fma(p, x, simd<float>(-2.8993394374847412109375f));

        return p;
    }

    template<>
    inline simd<double> detail::log_eval(simd<double> x) {
        // minimax polynomial for log(x) on [0.5, 0.999999] (base e)
        // generated by Sollya

        simd<double> p(-1.78002887866593395571612745698075741529464721679687);
        p = fma(p, x, simd<double>(15.237232714767539931699502631090581417083740234375));
        p = fma(p, x, simd<double>(-59.090975553227366390274255536496639251708984375));
        p = fma(p, x, simd<double>(137.151702012140276565332897007465362548828125));
        p = fma(p, x, simd<double>(-211.9912953584026809039642103016376495361328125));
        p = fma(p, x, simd<double>(229.705543798703303082220372743904590606689453125));
        p = fma(p, x, simd<double>(-178.9370318707593696672120131552219390869140625));
        p = fma(p, x, simd<double>(101.312817366204711788668646477162837982177734375));
        p = fma(p, x, simd<double>(-42.07977489397803338988524046726524829864501953125));
        p = fma(p, x, simd<double>(13.72036301968777394222342991270124912261962890625));
        p = fma(p, x, simd<double>(-3.24855235647029783052630591555498540401458740234375));

        return p;
    }


    template<>
    inline simd<float> log(simd<float> x) {
        const simd<float> ln2(0x1.62e430p-1f);

        simd<float> clamped = max(x, simd<float>(0.000001f));

        simd<float> exp = detail::get_exponent(clamped);
        simd<float> mant = detail::get_mantissa(clamped);

        simd<float> manto2 = mant * simd<float>(.5f);
        exp += simd<float>(1.0f);


        simd<float> log_m = detail::log_eval(manto2);

        return fma(exp, ln2, log_m);
    }

    template<>
    inline simd<double> log(simd<double> x) {
        const simd<double> ln2(0x1.62e42fefa39efp-1);


        simd<double> clamped = max(x, simd<double>(0.0000000000001));

        simd<double> exp = detail::get_exponent(clamped);
        simd<double> mant = detail::get_mantissa(clamped);

        simd<double> manto2 = mant * simd<double>(.5);
        exp += simd<double>(1.0);


        simd<double> log_m = detail::log_eval(manto2);

        return fma(exp, ln2, log_m);
    }

    template<>
    inline simd<float> sinh(simd<float> x) {
        simd<float> exp_x = exp(x);
        simd<float> exp_minus_x = exp(-x);
        return (exp_x - exp_minus_x) * simd<float>(.5f);
    }

    template<>
    inline simd<double> sinh(simd<double> x) {
        simd<double> exp_x = exp(x);
        simd<double> exp_minus_x = exp(-x);
        return (exp_x - exp_minus_x) * simd<double>(.5);
    }

    template<>
    inline simd<float> cosh(simd<float> x) {
        simd<float> exp_x = exp(x);
        simd<float> exp_minus_x = exp(-x);
        return (exp_x + exp_minus_x) * simd<float>(.5f);
    }

    template<>
    inline simd<double> cosh(simd<double> x) {
        simd<double> exp_x = exp(x);
        simd<double> exp_minus_x = exp(-x);
        return (exp_x + exp_minus_x) * simd<double>(.5);
    }

    template<>
    inline simd<float> tanh(simd<float> x) {
        simd<float> exp_2x = exp(x * simd<float>(2.0f));
        return (exp_2x - simd<float>(1.0f)) / (exp_2x + simd<float>(1.0f));
    }

    template<>
    inline simd<double> tanh(simd<double> x) {
        simd<double> exp_2x = exp(x * simd<double>(2.0));
        return (exp_2x - simd<double>(1.0)) / (exp_2x + simd<double>(1.0));
    }

    template<>
    inline simd<float> pow(simd<float> a, simd<float> b) {
        return exp(b * log(a));
    }

    template<>
    inline simd<double> pow(simd<double> a, simd<double> b) {
        return exp(b * log(a));
    }
}