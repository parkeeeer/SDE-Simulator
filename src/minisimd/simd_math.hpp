#pragma once

#include "types.hpp"


namespace sde::simd {
    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> fma(const simd<Num>& a, const simd<Num>& b, const simd<Num>& c);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> sqrt(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> abs(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> exp(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> log(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> sin(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> cos(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> tan(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> max(const simd<Num>& a, const simd<Num>& b);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> min(const simd<Num>& a, const simd<Num>& b);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> cosh(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> sinh(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> tanh(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> floor(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> ceil(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> round(const simd<Num>& x);

    //maybe
    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> atan(const simd<Num>& x);
}

namespace sde::simd::detail {

    template<concepts::FloatingPoint Num>
    struct trig_reduction {
        simd<Num> reduced;
        simd<Num> quadrant;
    };

    template<concepts::FloatingPoint Num>
    SIMD_INLINE trig_reduction<Num> cody_waite(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> sin_eval(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> cos_eval(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> exp_eval(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> log_eval(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> pow2(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> scalebn(const simd<Num>& x, const simd<Num>& n);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> get_mantissa(const simd<Num>& x);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> get_exponent(const simd<Num>& x);
}



//==========================implementations==========================//


namespace sde::simd {
#if SDE_HAS_AVX512
    inline simd<float> fma(const simd<float>& a, const simd<float>& b, const simd<float>& c) {
        return simd<float>(_mm512_fmadd_ps(a.v, b.v, c.v));
    }

    inline simd<double> fma(const simd<double>& a, const simd<double>& b, const simd<double>& c) {
        return simd<double>(_mm512_fmadd_pd(a.v, b.v, c.v));
    }
#elif SDE_HAS_AVX2
    inline simd<float> fma(const simd<float>& a, const simd<float>& b, const simd<float>& c) {
        return simd<float>(_m256_fmadd_ps(a.v,b.v,c.v));
    }

    inline simd<double> fma(const simd<double>& a, const simd<double>& b, const simd<double>& c) {
        return simd<double>(_m256_fmadd_pd(a.v,b.v,c.v));
    }
#elif SDE_HAS_NEON
    template<>
    inline simd<float> sde::simd::fma(const simd<float>& a, const simd<float>& b, const simd<float>& c) {
        return simd<float>(vmlaq_f32(c.v, a.v, b.v));
    }

    template<>
    inline simd<double> sde::simd::fma(const simd<double>& a, const simd<double>& b, const simd<double>& c) {
        return simd<double>(vmlaq_f64(c.v, a.v, b.v));
    }

    template<>
    inline simd<float> sde::simd::round(const simd<float>& x) {
        return simd<float>(vrndnq_f32(x.v));
    }

    template<>
    inline simd<double> sde::simd::round(const simd<double>& x) {
        return simd<double>(vrndnq_f64(x.v));
    }

    template<>
    inline simd<float> sde::simd::sqrt(const simd<float>& x) {
        return simd<float>(vsqrtq_f32(x.v));
    }

    template<>
    inline simd<double> sde::simd::sqrt(const simd<double>& x) {
        return simd<double>(vsqrtq_f64(x.v));
    }

    template<>
    inline simd<float> sde::simd::abs(const simd<float>& x) {
        return simd<float>(vabsq_f32(x.v));
    }

    template<>
    inline simd<double> sde::simd::abs(const simd<double>& x) {
        return simd<double>(vabsq_f64(x.v));
    }

    template<>
    inline simd<float> sde::simd::max(const simd<float>& a, const simd<float>& b) {
        return simd<float>(vmaxq_f32(a.v,b.v));
    }

    template<>
    inline simd<double> sde::simd::max(const simd<double>& a, const simd<double>& b) {
        return simd<double>(vmaxq_f64(a.v,b.v));
    }

    template<>
    inline simd<float> sde::simd::min(const simd<float>& a, const simd<float>& b) {
        return simd<float>(vminq_f32(a.v,b.v));
    }

    template<>
    inline simd<double> sde::simd::min(const simd<double>& a, const simd<double>& b) {
        return simd<double>(vminq_f64(a.v,b.v));
    }

    template<>
    inline simd<float> sde::simd::detail::pow2(const simd<float>& x) {
        int32x4_t x_to_int = vcvtq_s32_f32(x.v);

        int32x4_t bias = vaddq_s32(x_to_int, vdupq_n_s32(127));
        int32x4_t exponent = vshlq_n_s32(bias, 23);

        return simd<float>{vreinterpretq_f32_s32(exponent)};
    }

    template<>
    inline simd<double> sde::simd::detail::pow2(const simd<double>& x) {
        int64x2_t x_to_int = vcvtq_s64_f64(x.v);

        int64x2_t bias = vaddq_s64(x_to_int, vdupq_n_s64(1023));
        int64x2_t exponent = vshlq_n_s64(bias, 52);

        return simd<double>{vreinterpretq_f64_s64(exponent)};
    }

    template<>
    inline simd<float> detail::scalebn(const simd<float>& x, const simd<float>& n) {
        return x * pow2(n);
    }

    template<>
    inline simd<double> detail::scalebn(const simd<double>& x, const simd<double>& n) {
        return x * pow2(n);
    }

    template<>
    inline simd<float> detail::get_mantissa(const simd<float>& x) {
        uint32x4_t to_int = vreinterpretq_u32_f32(x.v);
        uint32x4_t mantissa_mask = vdupq_n_u32(0x807FFFFF);
        uint32x4_t exp_zero = vdupq_n_u32(127 << 23);
        uint32x4_t mantissa = vandq_u32(to_int, mantissa_mask);
        uint32x4_t result = vorrq_u32(exp_zero, mantissa);
        return simd<float>{vreinterpretq_f32_u32(result)};
    }

    template<>
    inline simd<double> detail::get_mantissa(const simd<double>& x) {
        uint64x2_t to_int = vreinterpretq_u64_f64(x.v);
        uint64x2_t mantissa_mask = vdupq_n_u64(0x800FFFFFFFFFFFFFULL);
        uint64x2_t exp_zero = vdupq_n_u64(1023ULL << 52);
        uint64x2_t mantissa = vandq_u64(to_int, mantissa_mask);
        uint64x2_t result = vorrq_u64(exp_zero, mantissa);
        return simd<double>{vreinterpretq_f64_u64(result)};
    }

    template<>
    inline simd<float> detail::get_exponent(const simd<float>& x) {
        uint32x4_t to_int = vreinterpretq_u32_f32(x.v);
        uint32x4_t exp_bits = vshrq_n_u32(to_int, 23);
        uint32x4_t exp_mask = vdupq_n_u32(0xFF);
        uint32x4_t biased_exp = vandq_u32(exp_bits, exp_mask);
        int32x4_t signed_exp = vreinterpretq_s32_u32(biased_exp);
        int32x4_t unbiased = vsubq_s32(signed_exp, vdupq_n_s32(127));
        return simd<float>{vcvtq_f32_s32(unbiased)};
    }

    template<>
    inline simd<double> detail::get_exponent(const simd<double>& x) {
        uint64x2_t to_int = vreinterpretq_u64_f64(x.v);
        uint64x2_t exp_bits = vshrq_n_u64(to_int, 52);
        uint64x2_t exp_mask = vdupq_n_u64(0x7FF);
        uint64x2_t biased_exp = vandq_u64(exp_bits, exp_mask);
        int64x2_t signed_exp = vreinterpretq_s64_u64(biased_exp);
        int64x2_t unbiased = vsubq_s64(signed_exp, vdupq_n_s64(1023));
        return simd<double>{vcvtq_f64_s64(unbiased)};
    }
#else
    inline simd<float> fma(const simd<float>& a, const simd<float>& b, const simd<float>& c) {
        return std::fmaf(a,b,c);
    }

    inline simd<double> fma(const simd<double>& a, const simd<double>& b, const simd<double>& c) {
        return std::fma(a,b,c);
    }
#endif

    template<>
    inline detail::trig_reduction<float> detail::cody_waite(const simd<float>& x) {
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
    inline detail::trig_reduction<double> detail::cody_waite(const simd<double>& x) {
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
    inline simd<float> detail::sin_eval(const simd<float>& x) {
        simd<float> x2 = x * x;

        simd<float> p(0x1.7d3bbcp-19f);
        p = fma(p, x2, simd<float>(-0x1.a06bbap-13f));
        p = fma(p, x2, simd<float>(0x1.11119ap-07f));
        p = fma(p, x2, simd<float>(-0x1.555556p-03f));
        p = p * x2;
        return fma(p,x,x);
    }

    template<>
    inline simd<float> detail::cos_eval(const simd<float>& x) {
        simd<float> x2 = x * x;

        simd<float> p(0x1.98e616p-16f);
        p = fma(p, x2, simd<float>(-0x1.6c06dcp-10f));
        p = fma(p, x2, simd<float>(0x1.55553cp-05f));
        p = fma(p, x2, simd<float>(-0x1.000000p-01f));
        return fma(p, x2, simd<float>(1.0f));
    }

    template<>
    inline simd<double> detail::sin_eval(const simd<double>& x) {
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
    inline simd<double> detail::cos_eval(const simd<double>& x) {
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
    inline simd<float> sin(const simd<float>& x) {
        simd<float> abs_x = abs(x);
        simd<float> signs = x & simd<float>(-0.0f);
        auto [xr, n] = detail::cody_waite(abs_x);

        auto tmp = select(n >= simd<float>(2.0f), simd<float>(1.0f), simd<float>(0.0f));

        auto swap_bit = n - simd<float>(2.0f) * tmp;

        auto flip_sign = tmp != simd<float>(0.0f);
        simd<float> result_sign = signs ^ select(flip_sign, simd<float>(-0.0f), simd<float>(0.0f));

        simd<float> sin_result = detail::sin_eval(xr);
        simd<float> cos_result = detail::cos_eval(xr);

        simd<float> result = select(swap_bit == simd<float>(0.0f), sin_result, cos_result);
        return result ^ result_sign;
    }

    template<>
    inline simd<float> cos(const simd<float>& x) {
        simd<float> abs_x = abs(x);
        //cos is even so no sign logic yay!!
        auto [xr, n] = detail::cody_waite(abs_x);

        auto tmp = select(n >= simd<float>(2.0f), simd<float>(1.0f), simd<float>(0.0f));
        auto swap_bit = n - simd<float>(2.0f) * tmp;

        auto flip_sign = tmp != simd<float>(0.0f);
        simd<float> result_sign = select(flip_sign, simd<float>(-0.0f), simd<float>(0.0f));

        simd<float> sin_result = detail::sin_eval(xr);
        simd<float> cos_result = detail::cos_eval(xr);

        simd<float> result = select(swap_bit == simd<float>(0.0f), cos_result, sin_result);
        return result ^ result_sign;
    }

    template<>
    inline simd<double> sin(const simd<double>& x) {
        simd<double> abs_x = abs(x);
        simd<double> signs = x & simd<double>(-0.0);
        auto [xr, n] = detail::cody_waite(abs_x);

        auto tmp = select(n >= simd<double>(2.0), simd<double>(1.0), simd<double>(0.0));

        auto swap_bit = n - simd<double>(2.0f) * tmp;

        auto flip_sign = tmp != simd<double>(0.0);
        simd<double> result_sign = signs ^ select(flip_sign, simd<double>(-0.0), simd<double>(0.0));

        simd<double> sin_result = detail::sin_eval(xr);
        simd<double> cos_result = detail::cos_eval(xr);

        simd<double> result = select(swap_bit == simd<double>(0.0), sin_result, cos_result);
        return result ^ result_sign;
    }

    template<>
    inline simd<double> cos(const simd<double>& x) {
        simd<double> abs_x = abs(x);
        //cos is even so no sign logic yay!!
        auto [xr, n] = detail::cody_waite(abs_x);

        auto tmp = select(n >= simd<double>(2.0), simd<double>(1.0), simd<double>(0.0));
        auto swap_bit = n - simd<double>(2.0) * tmp;

        auto flip_sign = tmp != simd<double>(0.0);
        simd<double> result_sign = select(flip_sign, simd<double>(-0.0), simd<double>(0.0));

        simd<double> sin_result = detail::sin_eval(xr);
        simd<double> cos_result = detail::cos_eval(xr);

        simd<double> result = select(swap_bit == simd<double>(0.0), cos_result, sin_result);
        return result ^ result_sign;
    }

    template<>
    inline simd<float> tan(const simd<float>& x) {
        simd<float> abs_x = abs(x);
        simd<float> signs = x & simd<float>(-0.0f);
        auto [xr, n] = detail::cody_waite(abs_x);

        simd<float> sin_result = detail::sin_eval(xr);
        simd<float> cos_result = detail::cos_eval(xr);

        auto tmp = select(n >= simd<float>(2.0f), simd<float>(1.0f), simd<float>(0.0f));
        auto swap_bit = n - simd<float>(2.0f) * tmp;

        auto flip_sign = tmp != simd<float>(0.0f);
        simd<float> sin_sign = signs ^ select(flip_sign, simd<float>(-0.0f), simd<float>(0.0f));
        simd<float> sin_val = select(swap_bit == simd<float>(0.0f), sin_result, cos_result);
        sin_val = sin_val ^ sin_sign;

        simd<float> cos_val = select(swap_bit == simd<float>(0.0f), cos_result, sin_result);
        cos_val = cos_val ^ select(flip_sign, simd<float>(-0.0f), simd<float>(0.0f));

        return sin_val / cos_val;
    }

    template<>
    inline simd<double> tan(const simd<double>& x) {
        simd<double> abs_x = abs(x);
        simd<double> signs = x & simd<double>(-0.0);
        auto [xr, n] = detail::cody_waite(abs_x);

        simd<double> sin_result = detail::sin_eval(xr);
        simd<double> cos_result = detail::cos_eval(xr);

        auto tmp = select(n >= simd<double>(2.0), simd<double>(1.0), simd<double>(0.0));
        auto swap_bit = n - simd<double>(2.0) * tmp;

        auto flip_sign = tmp != simd<double>(0.0);
        simd<double> sin_sign = signs ^ select(flip_sign, simd<double>(-0.0), simd<double>(0.0));
        simd<double> sin_val = select(swap_bit == simd<double>(0.0), sin_result, cos_result);
        sin_val = sin_val ^ sin_sign;

        simd<double> cos_val = select(swap_bit == simd<double>(0.0), cos_result, sin_result);
        cos_val = cos_val ^ select(flip_sign, simd<double>(-0.0), simd<double>(0.0));

        return sin_val / cos_val;
    }

    template<>
    inline simd<float> detail::exp_eval(const simd<float>& x) {
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
    inline simd<double> detail::exp_eval(const simd<double>& x) {
        simd<double> p(0x1.71ddf82db5bb4p-19);
        p = fma(p, x, simd<double>(0x1.27e4fb7789f5cp-14));
        p = fma(p, x, simd<double>(0x1.1ee9ebdb4b1c4p-10));
        p = fma(p, x, simd<double>(0x1.5d87fe78a6731p-07));
        p = fma(p, x, simd<double>(0x1.6c16c16bebd93p-05));
        p = fma(p, x, simd<double>(0x1.1111111110f8ap-03));  // ≈ 1/720
        p = fma(p, x, simd<double>(0x1.55555555554c3p-02));  // ≈ 1/6
        p = fma(p, x, simd<double>(0x1.0000000000000p-01));  // 1/2
        p = fma(p, x, simd<double>(1.0));
        p = fma(p, x, simd<double>(1.0));
        return p;
    }

    template<>
    inline simd<float> exp(const simd<float>& x) {
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

    inline simd<double> exp(const simd<double>& x) {
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
    inline simd<float> detail::log_eval(const simd<float>& x) {
        simd<float> p(0x1.0p-7f);
        p = fma(p, x, simd<float>(-0x1.0p-6f));
        p = fma(p, x, simd<float>(0x1.249250p-5f));
        p = fma(p, x, simd<float>(-0x1.555556p-5f));
        p = fma(p, x, simd<float>(0x1.999a0cp-4f));
        p = fma(p, x, simd<float>(-0x1.555556p-3f));
        p = fma(p, x, simd<float>(0x1.000000p-1f));
        p = p * x;
        p = fma(p, x, x);
        return p;
    }

    template<>
    inline simd<double> detail::log_eval(const simd<double>& x) {
        simd<double> p(0x1.c6a8093ac4f32p-8);
        p = fma(p, x, simd<double>(-0x1.bcb7b15200000p-7));
        p = fma(p, x, simd<double>(0x1.287a7636f4361p-6));
        p = fma(p, x, simd<double>(-0x1.bcb7b1526e50ep-6));
        p = fma(p, x, simd<double>(0x1.2492494229359p-5));
        p = fma(p, x, simd<double>(-0x1.7ffffffe6b25fp-5));
        p = fma(p, x, simd<double>(0x1.99999999992a4p-4));
        p = fma(p, x, simd<double>(-0x1.5555555555555p-3));
        p = fma(p, x, simd<double>(0x1.0000000000000p-1));
        p = p * x;
        p = fma(p, x, x);
        return p;
    }

    template<>
    inline simd<float> log(const simd<float>& x) {
        const simd<float> ln2(0x1.62e430p-1f);

        simd<float> clamped = max(x, simd<float>(0.000001f));

        simd<float> exp = detail::get_exponent(clamped);
        simd<float> mant = detail::get_mantissa(clamped);

        simd<float> r = mant - simd<float>(1.0f);
        simd<float> log_m = detail::log_eval(r);

        return fma(exp, ln2, log_m);
    }

    template<>
    inline simd<double> log(const simd<double>& x) {
        const simd<double> ln2(0x1.62e42fefa39efp-1);

        simd<double> clamped = max(x, simd<double>(0.0000000000001));

        simd<double> exp = detail::get_exponent(clamped);
        simd<double> mant = detail::get_mantissa(clamped);

        simd<double> r = mant - simd<double>(1.0);
        simd<double> log_m = detail::log_eval(r);

        return fma(exp, ln2, log_m);
    }

    template<>
    inline simd<float> sinh(const simd<float>& x) {
        simd<float> exp_x = exp(x);
        simd<float> exp_minus_x = exp(-x);
        return (exp_x - exp_minus_x) * simd<float>(.5f);
    }

    template<>
    inline simd<double> sinh(const simd<double>& x) {
        simd<double> exp_x = exp(x);
        simd<double> exp_minus_x = exp(-x);
        return (exp_x - exp_minus_x) * simd<double>(.5);
    }

    template<>
    inline simd<float> cosh(const simd<float>& x) {
        simd<float> exp_x = exp(x);
        simd<float> exp_minus_x = exp(-x);
        return (exp_x + exp_minus_x) * simd<float>(.5f);
    }

    template<>
    simd<double> cosh(const simd<double>& x) {
        simd<double> exp_x = exp(x);
        simd<double> exp_minus_x = exp(-x);
        return (exp_x + exp_minus_x) * simd<double>(.5);
    }

    template<>
    inline simd<float> tanh(const simd<float>& x) {
        simd<float> exp_2x = exp(x * simd<float>(2.0f));
        return (exp_2x - simd<float>(1.0f)) / (exp_2x + simd<float>(1.0f));
    }

    template<>
    simd<double> tanh(const simd<double>& x) {
        simd<double> exp_2x = exp(x * simd<double>(2.0));
        return (exp_2x - simd<double>(1.0)) / (exp_2x + simd<double>(1.0));
    }
}