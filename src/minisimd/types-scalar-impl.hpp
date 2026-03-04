#pragma once

namespace sde::simd {
    inline simd<float>::simd(float x) {
        v = x;
    }

    inline simd<float> simd<float>::load(const float* ptr) {
        return simd(*ptr);
    }

    inline void simd<float>::store(float* ptr) const {
        *ptr = v;
    }

    inline simd<float> simd<float>::operator+(simd other) const {
        return simd(v + other.v);
    }

    inline simd<float> simd<float>::operator-(simd other) const {
        return simd(v - other.v);
    }

    inline simd<float> simd<float>::operator*(simd other) const {
        return simd(v * other.v);
    }

    inline simd<float> simd<float>::operator/(simd other) const {
        return simd(v / other.v);
    }

    inline simd<float> simd<float>::operator-() const {
        return simd(-v);
    }

    inline simd<float>& simd<float>::operator+=(simd other) {
        v += other.v;
        return *this;
    }

    inline simd<float>& simd<float>::operator-=(simd other) {
        v -= other.v;
        return *this;
    }

    inline simd<float>& simd<float>::operator*=(simd other) {
        v *= other.v;
        return *this;
    }

    inline simd<float>& simd<float>::operator/=(simd other) {
        v /= other.v;
        return *this;
    }

    inline float simd<float>::operator[](size_t i) const {
        return v;
    }

    inline simd<double>::simd(double x) {
        v = x;
    }

    inline simd<double> simd<double>::load(const double* ptr) {
        return simd(*ptr);
    }

    inline void simd<double>::store(double* ptr) const {
        *ptr = v;
    }

    inline simd<double> simd<double>::operator+(simd other) const {
        return simd(v + other.v);
    }

    inline simd<double> simd<double>::operator-(simd other) const {
        return simd(v - other.v);
    }

    inline simd<double> simd<double>::operator*(simd other) const {
        return simd(v * other.v);
    }

    inline simd<double> simd<double>::operator/(simd other) const {
        return simd(v / other.v);
    }

    inline simd<double> simd<double>::operator-() const {
        return simd(-v);
    }

    inline simd<double>& simd<double>::operator+=(simd other) {
        v += other.v;
        return *this;
    }

    inline simd<double>& simd<double>::operator-=(simd other) {
        v -= other.v;
        return *this;
    }

    inline simd<double>& simd<double>::operator*=(simd other) {
        v *= other.v;
        return *this;
    }

    inline simd<double>& simd<double>::operator/=(simd other) {
        v /= other.v;
        return *this;
    }

    inline double simd<double>::operator[](size_t i) const {
        return v;
    }
}