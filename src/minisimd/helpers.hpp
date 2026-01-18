#pragma once

#include "types.hpp"


namespace sde::simd {
    template<simd_fp Num>
    SIMD_INLINE bool all(simd_mask<Num> mask);

    template<simd_fp Num>
    SIMD_INLINE bool any(simd_mask<Num> mask);

    template<simd_fp Num>
    SIMD_INLINE bool none(simd_mask<Num> mask);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> select(simd_mask<Num> mask, simd<Num> true_val, simd<Num> false_val);

    template<simd_fp Num>
    SIMD_INLINE simd_mask<Num> operator==(simd<Num> lhs, simd<Num> rhs);
    template<simd_fp Num>
    SIMD_INLINE simd_mask<Num> operator!=(simd<Num> lhs, simd<Num> rhs);
    template<simd_fp Num>
    SIMD_INLINE simd_mask<Num> operator<(simd<Num> lhs, simd<Num> rhs);
    template<simd_fp Num>
    SIMD_INLINE simd_mask<Num> operator>(simd<Num> lhs, simd<Num> rhs);
    template<simd_fp Num>
    SIMD_INLINE simd_mask<Num> operator>=(simd<Num> lhs, simd<Num> rhs);
    template<simd_fp Num>
    SIMD_INLINE simd_mask<Num> operator<=(simd<Num> lhs, simd<Num> rhs);

    template<simd_fp Num>
    SIMD_INLINE simd_mask<Num> operator&&(simd_mask<Num> lhs, simd_mask<Num> rhs);
    template<simd_fp Num>
    SIMD_INLINE simd_mask<Num> operator||(simd_mask<Num> lhs, simd_mask<Num> rhs);
    template<simd_fp Num>
    SIMD_INLINE simd_mask<Num> operator!(simd_mask<Num> mask);

    template<simd_fp Num>
    SIMD_INLINE simd<Num> operator&(simd<Num> lhs, simd<Num> rhs);
    template<simd_fp Num>
    SIMD_INLINE simd<Num> operator|(simd<Num> lhs, simd<Num> rhs);
    template<simd_fp Num>
    SIMD_INLINE simd<Num> operator^(simd<Num> lhs, simd<Num> rhs);
    template<simd_fp Num>
    SIMD_INLINE simd<Num> operator~(simd<Num> x);
}

#if SDE_HAS_AVX512

#elif SDE_HAS_AVX
#include "helpers-AVX-impl.hpp"
#elif SDE_HAS_NEON
#include "helpers-NEON-impl.hpp"
#else

#endif