#pragma once

#include "../Math.h"

namespace Deep {
    #ifdef DEEP_USE_SSE4_1
    Vec3::Vec3(float32 x, float32 y, float32 z) : sse_mm128(x, y, z, z) {
    }
    #else
    Vec3::Vec3(float32 x, float32 y, float32 z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    #endif

    Vec3& Vec3::Normalize() {
        #ifdef DEEP_USE_SSE4_1
        sse_mm128 = _mm_div_ps(sse_mm128, _mm_sqrt_ps(_mm_dp_ps(sse_mm128, sse_mm128, 0x7f)));
        return *this;
        #else
        return *this /= magnitude();
        #endif
    }
    Vec3 Vec3::normalized() const {
        Vec3 v = *this;
        return v.Normalize();
    }
    bool Vec3::IsNormalized(float tolerance) const {
        return Deep::Abs(sqrdMagnitude() - 1.0f) <= tolerance;
    }

    float32 Vec3::sqrdMagnitude() const {
        #ifdef DEEP_USE_SSE4_1
        return _mm_cvtss_f32(_mm_dp_ps(sse_mm128, sse_mm128, 0x7f));;
        #else
        return x * x + y * y + z * z;
        #endif
    }
    float32 Vec3::magnitude() const {
        #ifdef DEEP_USE_SSE4_1
        return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(sse_mm128, sse_mm128, 0x7f)));
        #else
        return Deep::Sqrt(sqrdMagnitude());
        #endif
    }

    bool operator!=(const Vec3& a, const Vec3& b) {
        #ifdef DEEP_USE_SSE4_1
        return (SSE_mm128::Equals(a.sse_mm128, b.sse_mm128).ToBooleanBitMask() & 0b111) != 0b111;
        #else
        return a.x != b.x || a.y != b.y || a.z != b.z;
        #endif
    }
    bool operator==(const Vec3& a, const Vec3& b) {
        return !(a != b);
    }

    Vec3& Vec3::operator+= (const Vec3& other) {
        #ifdef DEEP_USE_SSE4_1
        sse_mm128 += other.sse_mm128;
        #else
        x += other.x;
        y += other.y;
        z += other.z;
        #endif
        return *this;
    }

    Vec3 operator+ (Vec3 a, const Vec3& b) {
        return a += b;
    }

    Vec3& Vec3::operator-= (const Vec3& other) {
        #ifdef DEEP_USE_SSE4_1
        sse_mm128 -= other.sse_mm128;
        #else
        x -= other.x;
        y -= other.y;
        z -= other.z;
        #endif
        return *this;
    }

    Vec3 operator- (Vec3 a, const Vec3& b) {
        return a -= b;
    }

    Vec3 operator- (Vec3 a) {
        #ifdef DEEP_USE_SSE4_1
        a.sse_mm128 = -a.sse_mm128;
        #else
        a.x = -a.x;
        a.y = -a.y;
        a.z = -a.z;
        #endif
        return a;
    }

    Vec3& Vec3::operator*= (const Vec3& other) {
        #ifdef DEEP_USE_SSE4_1
        sse_mm128 *= other.sse_mm128;
        #else
        x *= other.x;
        y *= other.y;
        z *= other.z;
        #endif
        return *this;
    }
    Vec3 operator* (Vec3 a, const Vec3& b) {
        return a *= b;
    }

    Vec3& Vec3::operator*= (float32 other) {
        #ifdef DEEP_USE_SSE4_1
        sse_mm128 *= other;
        #else
        x *= other;
        y *= other;
        z *= other;
        #endif
        return *this;
    }

    Vec3 operator* (Vec3 a, float32 other) {
        return a *= other;
    }

    Vec3 operator* (float32 other, Vec3 a) {
        return a *= other;
    }

    Vec3& Vec3::operator/= (const Vec3& other) {
        #ifdef DEEP_USE_SSE4_1
        sse_mm128 /= other.sse_mm128;
        #else
        x /= other.x;
        y /= other.y;
        z /= other.z;
        #endif
        return *this;
    }
    Vec3 operator/ (Vec3 a, const Vec3& b) {
        return a /= b;
    }

    Vec3& Vec3::operator/= (float32 other) {
        #ifdef DEEP_USE_SSE4_1
        sse_mm128 /= other;
        #else
        x /= other;
        y /= other;
        z /= other;
        #endif
        return *this;
    }

    Vec3 operator/ (Vec3 a, float32 other) {
        return a /= other;
    }

    float32 Vec3::Dot(const Vec3& a, const Vec3& b) {
        #ifdef DEEP_USE_SSE4_1
        return _mm_cvtss_f32(_mm_dp_ps(a.sse_mm128, b.sse_mm128, 0x7f));
        #else
        return a.x * b.x + a.y * b.y + a.z * b.z;
        #endif
    }

    // TODO(randomuserhi): Vectorisation
    Vec3 operator* (const Mat4& m, const Vec3& v) {
        Vec3 _v;
        #ifdef DEEP_USE_SSE4_1
        _v.sse_mm128 = _mm_mul_ps(m.cols[0], _mm_shuffle_ps(m.cols[0], v.sse_mm128, _MM_SHUFFLE(0, 0, 0, 0)));
        _v.sse_mm128 = _mm_add_ps(_v.sse_mm128, _mm_mul_ps(m.cols[1], _mm_shuffle_ps(v.sse_mm128, v.sse_mm128, _MM_SHUFFLE(1, 1, 1, 1))));
        _v.sse_mm128 = _mm_add_ps(_v.sse_mm128, _mm_mul_ps(m.cols[2], _mm_shuffle_ps(v.sse_mm128, v.sse_mm128, _MM_SHUFFLE(2, 2, 2, 2))));
        _v.sse_mm128 = _mm_add_ps(_v.sse_mm128, m.cols[3]);
        #else
        float32 invW = 1.0f / (m.m30 * v.x + m.m31 * v.y + m.m32 * v.z + m.m33);
        _v.x = (m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03) * invW;
        _v.y = (m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13) * invW;
        _v.z = (m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23) * invW;
        #endif
        return _v;
    }
}