#pragma once

#include "../Math.h"

namespace Deep {
    Vec3& Vec3::Normalize() {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_div_ps(_internal, _mm_sqrt_ps(_mm_dp_ps(_internal, _internal, 0x7f)));
        return *this;
        #else
        return *this /= magnitude();
        #endif
    }
    Vec3 Vec3::normalized() const {
        Vec3 v{ x, y, z };
        return v.Normalize();
    }

    float32 Vec3::sqrdMagnitude() const {
        #ifdef DEEP_USE_SSE4_1
        return _mm_cvtss_f32(_mm_dp_ps(_internal, _internal, 0x7f));;
        #else
        return x * x + y * y + z * z;
        #endif
    }
    float32 Vec3::magnitude() const {
        #ifdef DEEP_USE_SSE4_1
        return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(_internal, _internal, 0x7f)));
        #else
        return Deep::Sqrt(sqrdMagnitude());
        #endif
    }

    bool operator!=(const Vec3& a, const Vec3& b) {
        #ifdef DEEP_USE_SSE4_1
        __m128i vec4i = _mm_castps_si128(_mm_cmpeq_ps(a._internal, b._internal));
        return (_mm_movemask_ps(_mm_castsi128_ps(vec4i)) & 0b111) != 0b111;
        #else
        return a.x != b.x || a.y != b.y || a.z != b.z;
        #endif
    }
    bool operator==(const Vec3& a, const Vec3& b) {
        return !(a != b);
    }

    Vec3& Vec3::operator+= (const Vec3& other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_add_ps(_internal, other._internal);
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
        _internal = _mm_sub_ps(_internal, other._internal);
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
        a._internal = _mm_sub_ps(_mm_setzero_ps(), a._internal);
        #else
        a.x = -a.x;
        a.y = -a.y;
        a.z = -a.z;
        #endif
        return a;
    }

    Vec3& Vec3::operator*= (const Vec3& other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_mul_ps(_internal, other._internal);
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
        _internal = _mm_mul_ps(_internal, _mm_set1_ps(other));
        #else
        x *= other;
        y *= other;
        z *= other;
        #endif
        return *this;
    }

    Vec3& Vec3::operator/= (const Vec3& other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_div_ps(_internal, other._internal);
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
        _internal = _mm_div_ps(_internal, _mm_set1_ps(other));
        #else
        x /= other;
        y /= other;
        z /= other;
        #endif
        return *this;
    }

    Vec3 operator* (Vec3 a, float32 other) {
        return a *= other;
    }

    Vec3 operator* (float32 other, Vec3 a) {
        return a *= other;
    }

    Vec3 operator/ (Vec3 a, float32 other) {
        return a /= other;
    }

    float32 Vec3::Dot(const Vec3& a, const Vec3& b) {
        #ifdef DEEP_USE_SSE4_1
        return _mm_cvtss_f32(_mm_dp_ps(a._internal, b._internal, 0x7f));
        #else
        return a.x * b.x + a.y * b.y + a.z * b.z;
        #endif
    }

    // TODO(randomuserhi): Vectorisation
    Vec3 operator* (const Mat3& m, const Vec3& v) {
        Vec3 _v;
        _v.x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z;
        _v.y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z;
        _v.z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z;
        return _v;
    }

    // TODO(randomuserhi): Vectorisation
    Vec3 operator* (const Mat4& m, const Vec3& v) {
        Vec3 _v;
        float32 invW = 1.0f / (m.m30 * v.x + m.m31 * v.y + m.m32 * v.z + m.m33);
        _v.x = (m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03) * invW;
        _v.y = (m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13) * invW;
        _v.z = (m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23) * invW;
        return _v;
    }
}