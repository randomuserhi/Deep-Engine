#pragma once

#include "../Math.h"

namespace Deep {
    #ifdef DEEP_USE_SSE4_1
    Vec4::Vec4(float32 x, float32 y, float32 z, float32 w) : sse_mm128(x, y, z, w) {
    }
    #else
    Vec4::Vec4(float32 x, float32 y, float32 z, float32 w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
    #endif

    Vec4& Vec4::Normalize() {
        #ifdef DEEP_USE_SSE4_1
        sse_mm128 = _mm_div_ps(sse_mm128, _mm_sqrt_ps(_mm_dp_ps(sse_mm128, sse_mm128, 0xff)));
        return *this;
        #else
        return *this /= magnitude();
        #endif
    }
    Vec4 Vec4::normalized() const {
        Vec4 v = *this;
        return v.Normalize();
    }
    bool Vec4::IsNormalized(float tolerance) const {
        return Deep::Abs(sqrdMagnitude() - 1.0f) <= tolerance;
    }

    float32 Vec4::sqrdMagnitude() const {
        #ifdef DEEP_USE_SSE4_1
        return _mm_cvtss_f32(_mm_dp_ps(sse_mm128, sse_mm128, 0xff));
        #else
        // NOTE(randomuserhi): brackets to keep consistent with vectorised version
        return (x * x + y * y) + (z * z + w * w);
        #endif
    }
    float32 Vec4::magnitude() const {
        #ifdef DEEP_USE_SSE4_1
        return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(sse_mm128, sse_mm128, 0xff)));
        #else
        return Deep::Sqrt(sqrdMagnitude());
        #endif
    }

    bool operator!=(const Vec4& a, const Vec4& b) {
        return a.sse_mm128 != b.sse_mm128;
    }
    bool operator==(const Vec4& a, const Vec4& b) {
        return !(a != b);
    }

    Vec4& Vec4::operator+= (const Vec4& other) {
        sse_mm128 += other.sse_mm128;
        return *this;
    }

    Vec4 operator+ (Vec4 a, const Vec4& b) {
        return a += b;
    }

    Vec4& Vec4::operator-= (const Vec4& other) {
        sse_mm128 -= other.sse_mm128;
        return *this;
    }

    Vec4 operator- (Vec4 a, const Vec4& b) {
        return a -= b;
    }

    Vec4 operator- (Vec4 a) {
        a.sse_mm128 = -a.sse_mm128;
        return a;
    }

    Vec4& Vec4::operator*= (const Vec4& other) {
        sse_mm128 *= other.sse_mm128;
        return *this;
    }
    Vec4 operator* (Vec4 a, const Vec4& b) {
        return a *= b;
    }

    Vec4& Vec4::operator*= (float32 other) {
        sse_mm128 *= other;
        return *this;
    }

    Vec4 operator* (Vec4 a, float32 other) {
        return a *= other;
    }

    Vec4 operator* (float32 other, Vec4 a) {
        return a *= other;
    }

    Vec4& Vec4::operator/= (const Vec4& other) {
        sse_mm128 /= other.sse_mm128;
        return *this;
    }
    Vec4 operator/ (Vec4 a, const Vec4& b) {
        return a /= b;
    }

    Vec4& Vec4::operator/= (float32 other) {
        sse_mm128 /= other;
        return *this;
    }

    Vec4 operator/ (Vec4 a, float32 other) {
        return a /= other;
    }

    float32 Vec4::Dot(const Vec4& a, const Vec4& b) {
        #ifdef DEEP_USE_SSE4_1 
        return _mm_cvtss_f32(_mm_dp_ps(a.sse_mm128, b.sse_mm128, 0xff));
        #else
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        #endif
    }

    Vec4 operator* (const Mat4& m, const Vec4& v) {
        Vec4 _v;
        #ifdef DEEP_USE_SSE4_1
        _v.sse_mm128 = _mm_mul_ps(m.cols[0], _mm_shuffle_ps(v.sse_mm128, v.sse_mm128, _MM_SHUFFLE(0, 0, 0, 0)));
        _v.sse_mm128 = _mm_add_ps(_v.sse_mm128, _mm_mul_ps(m.cols[1], _mm_shuffle_ps(v.sse_mm128, v.sse_mm128, _MM_SHUFFLE(1, 1, 1, 1))));
        _v.sse_mm128 = _mm_add_ps(_v.sse_mm128, _mm_mul_ps(m.cols[2], _mm_shuffle_ps(v.sse_mm128, v.sse_mm128, _MM_SHUFFLE(2, 2, 2, 2))));
        _v.sse_mm128 = _mm_add_ps(_v.sse_mm128, _mm_mul_ps(m.cols[3], _mm_shuffle_ps(v.sse_mm128, v.sse_mm128, _MM_SHUFFLE(3, 3, 3, 3))));
        #else
        _v.x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03 * v.w;
        _v.y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13 * v.w;
        _v.z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23 * v.w;
        _v.w = m.m30 * v.x + m.m31 * v.y + m.m32 * v.z + m.m33 * v.w;
        #endif
        return _v;
    }
}