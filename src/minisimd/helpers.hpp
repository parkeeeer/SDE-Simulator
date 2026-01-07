#pragma once
#include "concepts.hpp"
#include "types.hpp"


namespace sde::simd {
    template<concepts::FloatingPoint Num>
    SIMD_INLINE bool all(const simd_mask<Num>& mask);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE bool any(const simd_mask<Num>& mask);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE bool none(const simd_mask<Num>& mask);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> select(const simd_mask<Num>& mask, const simd<Num>& true_val, const simd<Num>& false_val);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd_mask<Num> operator==(const simd<Num>& lhs, const simd<Num>& rhs);
    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd_mask<Num> operator!=(const simd<Num>& lhs, const simd<Num>& rhs);
    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd_mask<Num> operator<(const simd<Num>& lhs, const simd<Num>& rhs);
    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd_mask<Num> operator>(const simd<Num>& lhs, const simd<Num>& rhs);
    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd_mask<Num> operator>=(const simd<Num>& lhs, const simd<Num>& rhs);
    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd_mask<Num> operator<=(const simd<Num>& lhs, const simd<Num>& rhs);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd_mask<Num> operator&&(const simd_mask<Num>& lhs, const simd_mask<Num>& rhs);
    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd_mask<Num> operator||(const simd_mask<Num>& lhs, const simd_mask<Num>& rhs);
    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd_mask<Num> operator!(const simd_mask<Num>& mask);

    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> operator&(const simd<Num>& lhs, const simd<Num>& rhs);
    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> operator|(const simd<Num>& lhs, const simd<Num>& rhs);
    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> operator^(const simd<Num>& lhs, const simd<Num>& rhs);
    template<concepts::FloatingPoint Num>
    SIMD_INLINE simd<Num> operator~(const simd<Num>& x);
}


#if SDE_HAS_AVX512

#elif SDE_HAS_AVX

#elif SDE_HAS_NEON
#include "helpers-NEON-impl.hpp"
#else

#endif