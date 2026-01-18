#pragma once

#include <immintrin.h>

#include "types.hpp"


namespace sde::simd {
    template<>
    inline simd_mask<float> sde::simd::operator!(simd_mask<float> x) {
        __m256 all_ones = _mm256_castsi256_ps(_mm256_set1_epi32(-1));
        return simd_mask<float>{_mm256_xor_ps(x.v, all_ones)};
    }
    template<>
    inline simd_mask<float> sde::simd::operator==(simd<float> lhs, simd<float> rhs) {
        return simd_mask<float>{_mm256_cmp_ps(lhs.v, rhs.v, _CMP_EQ_OQ)};
    }

    template<>
    inline simd_mask<float> sde::simd::operator!=(simd<float> lhs, simd<float> rhs) {
        return simd_mask<float>{_mm256_cmp_ps(lhs.v, rhs.v, _CMP_NEQ_OQ)};
    }

    template<>
    inline simd_mask<float> sde::simd::operator>=(simd<float> lhs, simd<float> rhs) {
        return simd_mask<float>{_mm256_cmp_ps(lhs.v, rhs.v, _CMP_GE_OQ)};
    }

    template<>
    inline simd_mask<float> sde::simd::operator>(simd<float> lhs, simd<float> rhs) {
        return simd_mask<float>{_mm256_cmp_ps(lhs.v, rhs.v, _CMP_GT_OQ)};
    }

    template<>
    inline simd_mask<float> sde::simd::operator<=(simd<float> lhs, simd<float> rhs) {
        return simd_mask<float>{_mm256_cmp_ps(lhs.v, rhs.v, _CMP_LE_OQ)};
    }

    template<>
    inline simd_mask<float> sde::simd::operator<(simd<float> lhs, simd<float> rhs) {
        return simd_mask<float>{_mm256_cmp_ps(lhs.v, rhs.v, _CMP_LT_OQ)};
    }

    template<>
    inline simd_mask<float> sde::simd::operator&&(simd_mask<float> lhs, simd_mask<float> rhs) {
        return simd_mask<float>{_mm256_and_ps(lhs.v, rhs.v)};
    }

    template<>
    inline simd_mask<float> sde::simd::operator||(simd_mask<float> lhs, simd_mask<float> rhs) {
        return simd_mask<float>{_mm256_or_ps(lhs.v, rhs.v)};
    }

    template<>
    inline bool sde::simd::all(simd_mask<float> mask) {
        return _mm256_movemask_ps(mask.v) != 0;
    }

    template<>
    inline bool sde::simd::any(simd_mask<float> mask) {
        return _mm256_movemask_ps(mask.v) == 0xFF;
    }

    template<>
    inline bool sde::simd::none(simd_mask<float> mask) {
        return _mm256_movemask_ps(mask.v) == 0;
    }

    template<>
    inline simd<float> sde::simd::select(simd_mask<float> mask, simd<float> true_val, simd<float> false_val) {
        return simd<float>{_mm256_blendv_ps(false_val.v, true_val.v, mask.v)};
    }

    template<>
    inline simd_mask<double> sde::simd::operator!(simd_mask<double> x) {
        __m256d all_ones = _mm256_castsi256_pd(_mm256_set1_epi32(-1));
        return simd_mask<double>{_mm256_xor_pd(x.v, all_ones)};
    }

    template<>
    inline simd_mask<double> sde::simd::operator&&(simd_mask<double> lhs, simd_mask<double> rhs) {
        return simd_mask<double>{_mm256_and_pd(lhs.v, rhs.v)};
    }

    template<>
    inline simd_mask<double> sde::simd::operator||(simd_mask<double> lhs, simd_mask<double> rhs) {
        return simd_mask<double>{_mm256_or_pd(lhs.v, rhs.v)};
    }

    template<>
    inline simd_mask<double> sde::simd::operator==(simd<double> lhs, simd<double> rhs) {
        return simd_mask<double>{_mm256_cmp_pd(lhs.v, rhs.v, _CMP_EQ_OQ)};
    }

    template<>
    inline simd_mask<double> sde::simd::operator!=(simd<double> lhs, simd<double> rhs) {
        return simd_mask<double>{_mm256_cmp_pd(lhs.v, rhs.v, _CMP_NEQ_OQ)};
    }

    template<>
    inline simd_mask<double> sde::simd::operator>=(simd<double> lhs, simd<double> rhs) {
        return simd_mask<double>{_mm256_cmp_pd(lhs.v, rhs.v, _CMP_GE_OQ)};
    }

    template<>
    inline simd_mask<double> sde::simd::operator>(simd<double> lhs, simd<double> rhs) {
        return simd_mask<double>{_mm256_cmp_pd(lhs.v, rhs.v, _CMP_GT_OQ)};
    }

    template<>
    inline simd_mask<double> sde::simd::operator<=(simd<double> lhs, simd<double> rhs) {
        return simd_mask<double>{_mm256_cmp_pd(lhs.v, rhs.v, _CMP_LE_OQ)};
    }

    template<>
    inline simd_mask<double> sde::simd::operator<(simd<double> lhs, simd<double> rhs) {
        return simd_mask<double>{_mm256_cmp_pd(lhs.v, rhs.v, _CMP_LT_OQ)};
    }

    template<>
    inline bool sde::simd::all(simd_mask<double> mask) {
        return _mm256_movemask_pd(mask.v) != 0;
    }

    template<>
    inline bool sde::simd::any(simd_mask<double> mask) {
        return _mm256_movemask_pd(mask.v) == 0xFF;
    }

    template<>
    inline bool sde::simd::none(simd_mask<double> mask) {
        return _mm256_movemask_pd(mask.v) == 0;
    }

    template<>
    inline simd<double> sde::simd::select(simd_mask<double> mask, simd<double> true_val, simd<double> false_val) {
        return simd<double>{_mm256_blendv_pd(false_val.v, true_val.v, mask.v)};
    }


    template<>
    inline simd<float> sde::simd::operator&(simd<float> a, simd<float> b) {
        return simd<float>{_mm256_and_ps(a.v, b.v)};
    }

    template<>
    inline simd<double> sde::simd::operator&(simd<double> a, simd<double> b) {
        return simd<double>{_mm256_and_pd(a.v, b.v)};
    }


    template<>
    inline simd<float> sde::simd::operator|(simd<float> a, simd<float> b) {
        return simd<float>{_mm256_or_ps(a.v, b.v)};
    }

    template<>
    inline simd<double> sde::simd::operator|(simd<double> a, simd<double> b) {
        return simd<double>{_mm256_or_pd(a.v, b.v)};
    }


    template<>
    inline simd<float> sde::simd::operator^(simd<float> a, simd<float> b) {
        return simd<float>{_mm256_xor_ps(a.v, b.v)};
    }

    template<>
    inline simd<double> sde::simd::operator^(simd<double> a, simd<double> b) {
        return simd<double>{_mm256_xor_pd(a.v, b.v)};
    }


    template<>
    inline simd<float> sde::simd::operator~(simd<float> a) {
        __m256 all_ones = _mm256_castsi256_ps(_mm256_set1_epi32(-1));
        return simd<float>(_mm256_xor_ps(a.v, all_ones));
    }

    template<>
    inline simd<double> sde::simd::operator~(simd<double> a) {
        __m256d all_ones = _mm256_castsi256_pd(_mm256_set1_epi32(-1));
        return simd<double>(_mm256_xor_pd(a.v, all_ones));
    }
}