#pragma once

namespace sde::simd {
    inline simd<float>::simd(float x) {
        v = _mm256_set1_ps(x);
    }

    inline simd<float> simd<float>::load(const float* ptr) {
        return simd{_mm256_load_ps(ptr)};
    }

    inline void simd<float>::store(float* ptr) const {
        _mm256_store_ps(ptr, v);
    }

    inline simd<float> simd<float>::operator+(simd rhs) const {
        return simd{_mm256_add_ps(v, rhs.v)};
    }

    inline simd<float> simd<float>::operator-(simd rhs) const {
        return simd{_mm256_sub_ps(v, rhs.v)};
    }

    inline simd<float> simd<float>::operator*(simd rhs) const {
        return simd{_mm256_mul_ps(v,rhs.v)};
    }

    inline simd<float> simd<float>::operator/(simd rhs) const {
        return simd{_mm256_div_ps(v, rhs.v)};
    }

    inline simd<float> simd<float>::operator-() const {
        __m256 sign_bit = _mm256_set1_ps(-0.0f);
        return simd{_mm256_xor_ps(v, sign_bit)};
    }

    inline simd<float>& simd<float>::operator+=(simd rhs) {
        v = _mm256_add_ps(v, rhs.v);
        return *this;
    }

    inline simd<float>& simd<float>::operator-=(simd rhs) {
        v = _mm256_sub_ps(v, rhs.v);
        return *this;
    }

    inline simd<float>& simd<float>::operator*=(simd rhs) {
        v = _mm256_mul_ps(v, rhs.v);
        return *this;
    }

    inline simd<float>& simd<float>::operator/=(simd rhs) {
        v = _mm256_div_ps(v, rhs.v);
        return *this;
    }

    inline float simd<float>::operator[](size_t i) const{
        alignas(32) float temp[8];
        _mm256_store_ps(temp, v);
        return temp[i];
    }

    inline simd<double>::simd(double x) {
        v = _mm256_set1_pd(x);
    }

    inline simd<double> simd<double>::load(const double* ptr) {
        v = _mm256_load_pd(ptr);
    }

    inline void simd<double>::store(double* ptr) const{
        _mm256_store_pd(ptr, v);
    }

    inline simd<double> simd<double>::operator+(simd rhs) const {
        return simd{_mm256_add_pd(v, rhs.v)};
    }

    inline simd<double> simd<double>::operator-(simd rhs) const {
        return simd{_mm256_sub_pd(v, rhs.v)};
    }

    inline simd<double> simd<double>::operator*(simd rhs) const {
        return simd{_mm256_mul_pd(v, rhs.v)};
    }

    inline simd<double> simd<double>::operator/(simd rhs) const {
        return simd{_mm256_div_pd(v, rhs.v)};
    }

    inline simd<double> simd<double>::operator-() const {
        __m256d sign_bit = _mm256_set1_pd(-0.0f);
        return simd{_mm256_xor_pd(v, sign_bit)};
    }

    inline simd<double>& simd<double>::operator+=(simd rhs) {
        v = _mm256_add_pd(v, rhs.v);
        return *this;
    }

    inline simd<double>& simd<double>::operator-=(simd rhs) {
        v = _mm256_sub_pd(v, rhs.v);
        return *this;
    }

    inline simd<double>& simd<double>::operator*=(simd rhs) {
        v = _mm256_mul_pd(v, rhs.v);
        return *this;
    }

    inline simd<double>& simd<double>::operator/=(simd rhs) {
        v = _mm256_div_pd(v, rhs.v);
        return *this;
    }

    inline double simd<double>::operator[](size_t i) const {
        alignas(32) double temp[4];
        store(temp);
        return temp[i];
    }
}
