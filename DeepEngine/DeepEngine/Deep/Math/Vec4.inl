#pragma once

#include "../Math.h"

namespace Deep {
    Vec4& Vec4::Normalize() {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_div_ps(_internal, _mm_sqrt_ps(_mm_dp_ps(_internal, _internal, 0xff)));
        return *this;
        #else
        return *this /= magnitude();
        #endif
    }
    Vec4 Vec4::normalized() const {
        Vec4 v{ x, y, z, w };
        return v.Normalize();
    }

    float32 Vec4::sqrdMagnitude() const {
        #ifdef DEEP_USE_SSE4_1
        return _mm_cvtss_f32(_mm_dp_ps(_internal, _internal, 0xff));
        #else
        // NOTE(randomuserhi): brackets to keep consistent with vectorised version
        return (x * x + y * y) + (z * z + w * w);
        #endif
    }
    float32 Vec4::magnitude() const {
        #ifdef DEEP_USE_SSE4_1
        return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(_internal, _internal, 0xff)));
        #else
        return Deep::Sqrt(sqrdMagnitude());
        #endif
    }

    bool operator!=(const Vec4& a, const Vec4& b) {
        #ifdef DEEP_USE_SSE4_1
        __m128i vec4i = _mm_castps_si128(_mm_cmpeq_ps(a._internal, b._internal));
        return (_mm_movemask_ps(_mm_castsi128_ps(vec4i)) & 0b111) != 0b111;
        #else
        return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
        #endif
    }
    bool operator==(const Vec4& a, const Vec4& b) {
        return !(a != b);
    }

    Vec4& Vec4::operator+= (const Vec4& other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_add_ps(_internal, other._internal);
        #else
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        #endif
        return *this;
    }

    Vec4 operator+ (Vec4 a, const Vec4& b) {
        return a += b;
    }

    Vec4& Vec4::operator-= (const Vec4& other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_sub_ps(_internal, other._internal);
        #else
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        #endif
        return *this;
    }

    Vec4 operator- (Vec4 a, const Vec4& b) {
        return a -= b;
    }

    Vec4 operator- (Vec4 a) {
        #ifdef DEEP_USE_SSE4_1
        a._internal = _mm_sub_ps(_mm_setzero_ps(), a._internal);
        #else
        a.x = -a.x;
        a.y = -a.y;
        a.z = -a.z;
        a.w = -a.w;
        #endif
        return a;
    }

    Vec4& Vec4::operator*= (const Vec4& other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_mul_ps(_internal, other._internal);
        #else
        x *= other.x;
        y *= other.y;
        z *= other.z;
        w *= other.w;
        #endif
        return *this;
    }
    Vec4 operator* (Vec4 a, const Vec4& b) {
        return a *= b;
    }

    Vec4& Vec4::operator*= (float32 other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_mul_ps(_internal, _mm_set1_ps(other));
        #else
        x *= other;
        y *= other;
        z *= other;
        w *= other;
        #endif
        return *this;
    }

    Vec4& Vec4::operator/= (const Vec4& other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_div_ps(_internal, other._internal);
        #else
        x /= other.x;
        y /= other.y;
        z /= other.z;
        w /= other.w;
        #endif
        return *this;
    }
    Vec4 operator/ (Vec4 a, const Vec4& b) {
        return a /= b;
    }

    Vec4& Vec4::operator/= (float32 other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_div_ps(_internal, _mm_set1_ps(other));
        #else
        x /= other;
        y /= other;
        z /= other;
        w /= other;
        #endif
        return *this;
    }

    Vec4 operator* (Vec4 a, float32 other) {
        return a *= other;
    }

    Vec4 operator* (float32 other, Vec4 a) {
        return a *= other;
    }

    Vec4 operator/ (Vec4 a, float32 other) {
        return a /= other;
    }

    float32 Vec4::Dot(const Vec4& a, const Vec4& b) {
        #ifdef DEEP_USE_SSE4_1 
        return _mm_cvtss_f32(_mm_dp_ps(a._internal, b._internal, 0xff));
        #else
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        #endif
    }

    Vec4 operator* (const Mat4& m, const Vec4& v) {
        Vec4 _v;
        #ifdef DEEP_USE_SSE4_1
        _v._internal = _mm_mul_ps(m.columns[0]._internal, _mm_shuffle_ps(v._internal, v._internal, _MM_SHUFFLE(0, 0, 0, 0)));
        _v._internal = _mm_add_ps(_v._internal, _mm_mul_ps(m.columns[1]._internal, _mm_shuffle_ps(v._internal, v._internal, _MM_SHUFFLE(1, 1, 1, 1))));
        _v._internal = _mm_add_ps(_v._internal, _mm_mul_ps(m.columns[2]._internal, _mm_shuffle_ps(v._internal, v._internal, _MM_SHUFFLE(2, 2, 2, 2))));
        _v._internal = _mm_add_ps(_v._internal, _mm_mul_ps(m.columns[3]._internal, _mm_shuffle_ps(v._internal, v._internal, _MM_SHUFFLE(3, 3, 3, 3))));
        #else
        _v.x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03 * v.w;
        _v.y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13 * v.w;
        _v.z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23 * v.w;
        _v.w = m.m30 * v.x + m.m31 * v.y + m.m32 * v.z + m.m33 * v.w;
        #endif
        return _v;
    }
}