/*

This file uses xoshiro256+ as a pseudorandom number generator,
which is a good all-purpose PRNG for floating point values. 
See http://prng.di.unimi.it/ for more details.
I also use splitmix64 as a seed generator for xoshiro256+.
nothing too interesting here, it also has a utility to convert
the uniform output to normal distribution using the Box-Muller transform.
I might change to a different transformation later that is faster
than Box-Muller, but this is fine for now. I added this comment because it kind
of uses a lot of bitwise operations and magic numbers, so it might be confusing

*/

#pragma once

#include <cstdint>
#include <cmath>
#include <limits>
#include <bit>

namespace sde_utils {


    struct SplitMix64 {
        uint64_t x; explicit SplitMix64(uint64_t seed): x(seed) {}
        uint64_t next() {
            uint64_t z = (x += 0x9E3779B97F4A7C15ull);
            z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
            z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
            return z ^ (z >> 31);
        }   
    };

    struct Xoshiro256Plus {
        uint64_t s[4];
        static inline uint64_t rot_left(uint64_t v, int k){ return (v<<k)|(v>>(64-k)); }

        explicit Xoshiro256Plus(uint64_t seed, uint64_t stream=0) {
            SplitMix64 sm(seed ^ (0x9E3779B97F4A7C15ull * (stream+1)));
            for (int i=0;i<4;++i) s[i]=sm.next();
        }
        uint64_t next_u64() {
            uint64_t const result = s[0] + s[3];
            uint64_t const t = s[1] << 17;
            s[2] ^= s[0]; 
            s[3] ^= s[1]; 
            s[1] ^= s[2]; 
            s[0] ^= s[3];
            s[2] ^= t;   
            s[3] = rot_left(s[3], 45);
            return result;
        }

        void jump(){

        }

        void long_jump(){

        }
    };

    struct BoxMuller{
        Xoshiro256Plus rng;
        

        
    };
}