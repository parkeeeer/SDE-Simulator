#pragma once

#include <stdexcept>

namespace sde::simd {
    inline simd<float>::simd(float x) {
        v = vdupq_n_f32(x);
    }

    inline simd<float> simd<float>::load(const float* ptr) {
        return simd(vld1q_f32(ptr));
    }

    inline void simd<float>::store(float* ptr) const {
        vst1q_f32(ptr, v);
    }

    inline simd<float> simd<float>::operator+(const simd& other) const {
        return simd(vaddq_f32(v, other.v));
    }

    inline simd<float> simd<float>::operator-(const simd& other) const {
        return simd(vsubq_f32(v, other.v));
    }

    inline simd<float> simd<float>::operator*(const simd& other) const {
        return simd(vmulq_f32(v, other.v));
    }

    inline simd<float> simd<float>::operator/(const simd& other) const {
        return simd(vdivq_f32(v, other.v));
    }

    inline simd<float> simd<float>::operator-() const {
        return simd(vnegq_f32(v));
    }

    inline simd<float>& simd<float>::operator+=(const simd& other){
        v = vaddq_f32(v, other.v);
        return *this;
    }

    inline simd<float>& simd<float>::operator-=(const simd& other){
        v = vsubq_f32(v, other.v);
        return *this;
    }

    inline simd<float>& simd<float>::operator*=(const simd& other) {
        v = vmulq_f32(v, other.v);
        return *this;
    }

    inline simd<float>& simd<float>::operator/=(const simd& other) {
        v = vdivq_f32(v, other.v);
        return *this;
    }

    inline float simd<float>::operator[](size_t i) const {
        switch (i) {
            case 0: return vgetq_lane_f32(v, 0);
            case 1: return vgetq_lane_f32(v, 1);
            case 2: return vgetq_lane_f32(v, 2);
            case 3: return vgetq_lane_f32(v, 3);
            default: throw std::runtime_error("Invalid index");
        }
    }

    inline simd<double>::simd(double x) {
        v = vdupq_n_f64(x);
    }

    inline simd<double> simd<double>::load(const double* ptr) {
        return simd(vld1q_f64(ptr));
    }

    inline void simd<double>::store(double* ptr) const {
        vst1q_f64(ptr, v);
    }

    inline simd<double> simd<double>::operator+(const simd& other) const {
        return simd(vaddq_f64(v, other.v));
    }

    inline simd<double> simd<double>::operator-(const simd& other) const {
        return simd(vsubq_f64(v, other.v));
    }

    inline simd<double> simd<double>::operator*(const simd& other) const {
        return simd(vmulq_f64(v, other.v));
    }

    inline simd<double> simd<double>::operator/(const simd& other) const {
        return simd(vdivq_f64(v, other.v));
    }

    inline simd<double> simd<double>::operator-() const {
        return simd(vnegq_f64(v));
    }

    inline simd<double>& simd<double>::operator+=(const simd& other){
        v = vaddq_f64(v, other.v);
        return *this;
    }

    inline simd<double>& simd<double>::operator-=(const simd& other) {
        v = vsubq_f64(v, other.v);
        return *this;
    }

    inline simd<double>& simd<double>::operator*=(const simd& other) {
        v = vmulq_f64(v, other.v);
        return *this;
    }

    inline simd<double>& simd<double>::operator/=(const simd& other) {
        v = vdivq_f64(v, other.v);
        return *this;
    }

    inline double simd<double>::operator[](size_t i) const {
        switch (i) {
            case 0: return vgetq_lane_f64(v, 0);
            case 1: return vgetq_lane_f64(v, 1);
            default: throw std::runtime_error("Invalid index");
        }
    }
}