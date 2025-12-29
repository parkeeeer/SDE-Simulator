#pragma once

#ifdef _MSC_VER
#define SIMD_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define SIMD_INLINE inline __attribute__((always_inline))
#else
#define SIMD_INLINE inline
#endif

#ifdef __AVX512F__
#define HAS_AVX512 true
#else
#define HAS_AVX512 false
#endif

#ifdef __AVX2__
#define HAS_AVX2 true
#else
#define HAS_AVX2 false
#endif

#ifdef __SSE2__
#define HAS_SSE2 true
#else
#define HAS_SSE2 false
#endif