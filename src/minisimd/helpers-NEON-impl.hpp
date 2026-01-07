#pragma once

#include<arm_neon.h>

namespace sde::simd {
    template<>
    inline simd_mask<float> sde::simd::operator!(const simd_mask<float>& x) {
        return {vmvnq_u32(x.v)};
    }
    template<>
    inline simd_mask<float> sde::simd::operator==(const simd<float>& lhs, const simd<float>& rhs) {
        return {vceqq_f32(lhs.v, rhs.v)};
    }

    template<>
    inline simd_mask<float> sde::simd::operator!=(const simd<float>& lhs, const simd<float>& rhs) {
        return !(lhs == rhs);
    }

    template<>
    inline simd_mask<float> sde::simd::operator>=(const simd<float>& lhs, const simd<float>& rhs) {
        return {vcgeq_f32(lhs.v, rhs.v)};
    }

    template<>
    inline simd_mask<float> sde::simd::operator>(const simd<float>& lhs, const simd<float>& rhs) {
        return {vcgtq_f32(lhs.v, rhs.v)};
    }

    template<>
    inline simd_mask<float> sde::simd::operator<=(const simd<float>& lhs, const simd<float>& rhs) {
        return {vcleq_f32(lhs.v, rhs.v)};
    }

    template<>
    inline simd_mask<float> sde::simd::operator<(const simd<float>& lhs, const simd<float>& rhs) {
        return {vcltq_f32(lhs.v, rhs.v)};
    }

    template<>
    inline simd_mask<float> sde::simd::operator&&(const simd_mask<float>& lhs, const simd_mask<float>& rhs) {
        return {vandq_u32(lhs.v, rhs.v)};
    }

    template<>
    inline simd_mask<float> sde::simd::operator||(const simd_mask<float>& lhs, const simd_mask<float>& rhs) {
        return {vorrq_u32(lhs.v, rhs.v)};
    }

    template<>
    inline bool sde::simd::all(const simd_mask<float>& mask) {
        uint32_t min_v = vminvq_u32(mask.v);
        return min_v != 0;
    }

    template<>
    inline bool sde::simd::any(const simd_mask<float>& mask) {
        uint32_t max_v = vmaxvq_u32(mask.v);
        return max_v != 0;
    }

    template<>
    inline bool sde::simd::none(const simd_mask<float>& mask) {
        return !any(mask);
    }

    template<>
    inline simd<float> sde::simd::select(const simd_mask<float>& mask, const simd<float>& true_val, const simd<float>& false_val) {
        return simd<float>(vbslq_f32(mask.v, true_val.v, false_val.v));
    }

    template<>
    inline simd_mask<double> sde::simd::operator!(const simd_mask<double>& x) {
        uint64x2_t ones = vdupq_n_u64(0xFFFFFFFFFFFFFFFFULL);
        return {veorq_u64(x.v, ones)};
    }

    template<>
    inline simd_mask<double> sde::simd::operator&&(const simd_mask<double>& lhs, const simd_mask<double>& rhs) {
        return {vandq_u64(lhs.v, rhs.v)};
    }

    template<>
    inline simd_mask<double> sde::simd::operator||(const simd_mask<double>& lhs, const simd_mask<double>& rhs) {
        return {vorrq_u64(lhs.v, rhs.v)};
    }

    template<>
    inline simd_mask<double> sde::simd::operator==(const simd<double>& lhs, const simd<double>& rhs) {
        return {vceqq_f64(lhs.v, rhs.v)};
    }

    template<>
    inline simd_mask<double> sde::simd::operator!=(const simd<double>& lhs, const simd<double>& rhs) {
        return !(lhs == rhs);
    }

    template<>
    inline simd_mask<double> sde::simd::operator>=(const simd<double>& lhs, const simd<double>& rhs) {
        return {vcgeq_f64(lhs.v, rhs.v)};
    }

    template<>
    inline simd_mask<double> sde::simd::operator>(const simd<double>& lhs, const simd<double>& rhs) {
        return {vcgtq_f64(lhs.v, rhs.v)};
    }

    template<>
    inline simd_mask<double> sde::simd::operator<=(const simd<double>& lhs, const simd<double>& rhs) {
        return {vcleq_f64(lhs.v, rhs.v)};
    }

    template<>
    inline simd_mask<double> sde::simd::operator<(const simd<double>& lhs, const simd<double>& rhs) {
        return {vcltq_f64(lhs.v, rhs.v)};
    }

    template<>
    inline bool sde::simd::all(const simd_mask<double>& mask) {
        return vgetq_lane_u64(mask.v, 0) != 0 && vgetq_lane_u64(mask.v, 1) != 0;
    }

    template<>
    inline bool sde::simd::any(const simd_mask<double>& mask) {
        return vgetq_lane_u64(mask.v, 0) != 0 || vgetq_lane_u64(mask.v, 1) != 0;
    }

    template<>
    inline bool sde::simd::none(const simd_mask<double>& mask) {
        return !any(mask);
    }

    template<>
    inline simd<double> sde::simd::select(const simd_mask<double>& mask, const simd<double>& true_val, const simd<double>& false_val) {
        return simd<double>(vbslq_f64(mask.v, true_val.v, false_val.v));
    }


    template<>
    inline simd<float> sde::simd::operator&(const simd<float>& a, const simd<float>& b) {
        // Reinterpret as integer, AND, reinterpret back
        uint32x4_t a_int = vreinterpretq_u32_f32(a.v);
        uint32x4_t b_int = vreinterpretq_u32_f32(b.v);
        uint32x4_t result = vandq_u32(a_int, b_int);
        return simd<float>{vreinterpretq_f32_u32(result)};
    }

    template<>
    inline simd<double> sde::simd::operator&(const simd<double>& a, const simd<double>& b) {
        uint64x2_t a_int = vreinterpretq_u64_f64(a.v);
        uint64x2_t b_int = vreinterpretq_u64_f64(b.v);
        uint64x2_t result = vandq_u64(a_int, b_int);
        return simd<double>{vreinterpretq_f64_u64(result)};
    }


    template<>
    inline simd<float> sde::simd::operator|(const simd<float>& a, const simd<float>& b) {
        uint32x4_t a_int = vreinterpretq_u32_f32(a.v);
        uint32x4_t b_int = vreinterpretq_u32_f32(b.v);
        uint32x4_t result = vorrq_u32(a_int, b_int);
        return simd<float>{vreinterpretq_f32_u32(result)};
    }

    template<>
    inline simd<double> sde::simd::operator|(const simd<double>& a, const simd<double>& b) {
        uint64x2_t a_int = vreinterpretq_u64_f64(a.v);
        uint64x2_t b_int = vreinterpretq_u64_f64(b.v);
        uint64x2_t result = vorrq_u64(a_int, b_int);
        return simd<double>{vreinterpretq_f64_u64(result)};
    }


    template<>
    inline simd<float> sde::simd::operator^(const simd<float>& a, const simd<float>& b) {
        uint32x4_t a_int = vreinterpretq_u32_f32(a.v);
        uint32x4_t b_int = vreinterpretq_u32_f32(b.v);
        uint32x4_t result = veorq_u32(a_int, b_int);
        return simd<float>{vreinterpretq_f32_u32(result)};
    }

    template<>
    inline simd<double> sde::simd::operator^(const simd<double>& a, const simd<double>& b) {
        uint64x2_t a_int = vreinterpretq_u64_f64(a.v);
        uint64x2_t b_int = vreinterpretq_u64_f64(b.v);
        uint64x2_t result = veorq_u64(a_int, b_int);
        return simd<double>(vreinterpretq_f64_u64(result));
    }


    template<>
    inline simd<float> sde::simd::operator~(const simd<float>& a) {
        uint32x4_t a_int = vreinterpretq_u32_f32(a.v);
        uint32x4_t result = vmvnq_u32(a_int);
        return simd<float>{vreinterpretq_f32_u32(result)};
    }

    template<>
    inline simd<double> sde::simd::operator~(const simd<double>& a) {
        uint64x2_t a_int = vreinterpretq_u64_f64(a.v);
        uint64x2_t all_ones = vdupq_n_u64(~0ULL);
        uint64x2_t result = veorq_u64(a_int, all_ones);
        return simd<double>{vreinterpretq_f64_u64(result)};
    }
}
