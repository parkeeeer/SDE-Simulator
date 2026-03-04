add_library(SDE_SIMD INTERFACE)
target_include_directories(SDE_SIMD INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/include)

target_compile_definitions(SDE_SIMD INTERFACE HAS_AVX2)
if(MSVC)
    target_compile_options(SDE_SIMD INTERFACE /arch:AVX2)
else()
    target_compile_options(SDE_SIMD INTERFACE -mavx2)
endif()



#[[
include(CheckCXXSourceRuns)

check_cxx_source_runs("
#include <immintrin.h>
int main() {
    __m512 a = _mm512_setzero_ps();
    return 0;
}
" HAS_AVX512)

check_cxx_source_runs("
#include <immintrin.h>
int main() {
    __m256 a = _mm256_setzero_ps();
    return 0;
}
" HAS_AVX2)

check_cxx_source_runs("
#include <arm_neon.h>
int main() {
    float32x4_t a = vdupq_n_f32(0.0f);
    return 0;
}
" HAS_NEON)

if(HAS_AVX512)
    if(MSVC)
        target_compile_options(SDE_SIMD INTERFACE /arch:AVX512)
    else()
        target_compile_options(SDE_SIMD INTERFACE -mavx512f)
    endif()
    target_compile_definitions(SDE_SIMD INTERFACE HAS_AVX512)
elseif(HAS_AVX2)
    if(MSVC)
        target_compile_options(SDE_SIMD INTERFACE /arch:AVX2)
    else()
        target_compile_options(SDE_SIMD INTERFACE -mavx2)
    endif()
    target_compile_definitions(SDE_SIMD INTERFACE HAS_AVX2)
elseif(HAS_NEON)
    target_compile_options(SDE_SIMD INTERFACE -mfpu=neon)
    target_compile_definitions(SDE_SIMD INTERFACE HAS_NEON)
endif()

message(STATUS "HAS_AVX2: ${HAS_AVX2}")
message(STATUS "HAS_AVX512: ${HAS_AVX512}")
]]
