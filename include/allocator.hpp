    #pragma once

    #include "concepts.hpp"
    #include <cstdlib>



    namespace sde::memory{

        constexpr size_t get_simd_alignment() {
            #if defined(__AVX__) || defined(__AVX2__)
            return 32;
            #elif defined(__AVX512F__)
            return 64;
            #elif defined(__SSE4_2__) || defined(__SSE4_1__) || defined(__SSSE3__) || defined(__SSE3__) || defined(__SSE2__) || defined(__SSE__)
            return 16;
            #elif defined(__ARM_NEON) || defined(__ARM_NEON__)
            return 16;
            #else
            return 32;
            #endif
        }

        template<sde::concepts::fp_or_simd T>
        struct aligned_allocator {
            using value_type = T;
            T* allocate(size_t n) {

                //needs to be correctly aligned!!!!!
                constexpr size_t alignment = get_simd_alignment();
                size_t aligned_size = ((n * sizeof(T) + alignment - 1) / alignment) * alignment;
                void* p;
#ifdef _WIN32
                p = _aligned_malloc(aligned_size, alignment);
#else
                posix_memalign(&p, alignment, aligned_size);
#endif
                if (!p) throw std::bad_alloc{};
                return static_cast<T*>(p);
            }

            void deallocate(T* p, size_t) {
#ifdef _WIN32
                _aligned_free(p);
#else
                free(p);
#endif
            }

            bool operator==(const aligned_allocator&) const noexcept { return true; }
            bool operator!=(const aligned_allocator&) const noexcept { return false; }
        };

        template<sde::concepts::fp_or_simd T>
        using aligned_vector = std::vector<T, aligned_allocator<T>>;

    }