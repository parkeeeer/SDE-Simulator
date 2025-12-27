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

namespace sde::rng {


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

    	//stl compliance stuff:
    	using result_type = uint64_t;
    	static constexpr result_type min() {return 0;}
    	static constexpr result_type max() {return std::numeric_limits<result_type>::max();}
    	result_type operator()(){return next_u64();}


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

        void jump(void) {
	        static const uint64_t JUMP[] = { 0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c };

	        uint64_t s0 = 0;
	        uint64_t s1 = 0;
	        uint64_t s2 = 0;
	        uint64_t s3 = 0;
	        for(int i = 0; i < sizeof JUMP / sizeof *JUMP; i++){
		        for(int b = 0; b < 64; b++) {
			        if (JUMP[i] & UINT64_C(1) << b) {
				        s0 ^= s[0];
				        s1 ^= s[1];
				        s2 ^= s[2];
				        s3 ^= s[3];
			        }
			    next_u64();
                }
		    }
	        s[0] = s0;
	        s[1] = s1;
	        s[2] = s2;
	        s[3] = s3;
        }

        void long_jump(void) {
	        static const uint64_t LONG_JUMP[] = { 0x76e15d3efefdcbbf, 0xc5004e441c522fb3, 0x77710069854ee241, 0x39109bb02acbe635 };

	        uint64_t s0 = 0;
	        uint64_t s1 = 0;
	        uint64_t s2 = 0;
	        uint64_t s3 = 0;
	        for(int i = 0; i < sizeof LONG_JUMP / sizeof *LONG_JUMP; i++){
		        for(int b = 0; b < 64; b++) {
			        if (LONG_JUMP[i] & UINT64_C(1) << b) {
				        s0 ^= s[0];
				        s1 ^= s[1];
				        s2 ^= s[2];
				        s3 ^= s[3];
			        }
			    next_u64();
                }
		    }
	        s[0] = s0;
	        s[1] = s1;
	        s[2] = s2;
	        s[3] = s3;
        }
    };

    inline float u64_to_float(uint64_t x){
        constexpr float scale = 1.0f / 16777216.0f;
        return static_cast<float>(x >> 40) * scale;
    }

    inline double u64_to_double(uint64_t x){
        constexpr double scale = 1.0 / 9007199254740992.0;
        return static_cast<double>(x >> 11) * scale;
    }



    
}