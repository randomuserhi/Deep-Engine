#pragma once

namespace Deep {
    SSE_m128i::SSE_m128i(int32 x, int32 y, int32 z, int32 w) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_set_epi32(w, z, y, x);
        #else
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
        #endif
    }
    SSE_m128i::SSE_m128i(Type _internal) : _internal(_internal) {
    }

    SSE_m128i::operator Type() const {
        return _internal;
    }

    SSE_m128 SSE_m128i::ToFloat() const {
        #ifdef DEEP_USE_SSE
        return _mm_cvtepi32_ps(_internal);
        #else
        return SSE_m128{
            static_cast<float32>(x),
            static_cast<float32>(y),
            static_cast<float32>(z),
            static_cast<float32>(w)
        };
        #endif
    }
    SSE_m128 SSE_m128i::ReinterpretAsFloat() const {
        #ifdef DEEP_USE_SSE
        return _mm_castsi128_ps(_internal);
        #else
        return *reinterpret_cast<const SSE_m128*>(this);
        #endif
    }

    SSE_m128i SSE_m128i::Replicate(int value) {
        #ifdef DEEP_USE_SSE
        return _mm_set1_epi32(value);
        #else
        return SSE_m128i{
            value,
            value,
            value,
            value
        };
        #endif
    }

    int32 SSE_m128i::ToBooleanBitMask() const {
        #ifdef DEEP_USE_SSE
        return _mm_movemask_ps(_mm_castsi128_ps(_internal));
        #else
        return (x >> 31) | ((y >> 31) << 1) | ((z >> 31) << 2) | ((w >> 31) << 3);
        #endif
    }

    SSE_m128i SSE_m128i::Xor(SSE_m128iArg a, SSE_m128iArg b) {
        #ifdef DEEP_USE_SSE
        return _mm_xor_si128(a, b);
        #else
        return SSE_m128i{
            a.x ^ b.x,
            a.y ^ b.y,
            a.z ^ b.z,
            a.w ^ b.w
        };
        #endif
    }

    SSE_m128i SSE_m128i::And(SSE_m128iArg a, SSE_m128iArg b) {
        #ifdef DEEP_USE_SSE
        return _mm_and_si128(a, b);
        #else
        return SSE_m128i{
            a.x & b.x,
            a.y & b.y,
            a.z & b.z,
            a.w & b.w
        };
        #endif
    }

    SSE_m128i SSE_m128i::Equals(SSE_m128iArg a, SSE_m128iArg b) {
        #ifdef DEEP_USE_SSE
        return _mm_cmpeq_epi32(a, b);
        #else
        return SSE_m128i{
            a.x == b.x ? (int32)0xffffffff : 0,
            a.y == b.y ? (int32)0xffffffff : 0,
            a.z == b.z ? (int32)0xffffffff : 0,
            a.w == b.w ? (int32)0xffffffff : 0
        };
        #endif
    }

    template <const uint Count>
    SSE_m128i SSE_m128i::LogicalShiftLeft() const {
        static_assert(Count <= 31, "Invalid shift");

        #ifdef DEEP_USE_SSE
        return _mm_slli_epi32(_internal, Count);
        #else
        return SSE_m128i{
            x << Count,
            y << Count,
            z << Count,
            w << Count
        };
        #endif
    }

    template <const uint Count>
    SSE_m128i SSE_m128i::LogicalShiftRight() const {
        static_assert(Count <= 31, "Invalid shift");

        #ifdef DEEP_USE_SSE
        return _mm_srli_epi32(_internal, Count);
        #else
        return SSE_m128i{
            x >> Count,
            y >> Count,
            z >> Count,
            w >> Count
        };
        #endif
    }

    SSE_m128i SSE_m128i::Select(SSE_m128iArg a, SSE_m128iArg b, SSE_m128iArg control) {
        #ifdef DEEP_USE_SSE4_1
        return _mm_castps_si128(_mm_blendv_ps(_mm_castsi128_ps(a), _mm_castsi128_ps(b), _mm_castsi128_ps(control)));
        #else
        return SSE_m128i{
            control.x ? b.x : a.x,
            control.y ? b.y : a.y,
            control.z ? b.z : a.z,
            control.w ? b.w : a.w
        };
        #endif
    }

    bool operator!=(SSE_m128iArg a, SSE_m128iArg b) {
        #ifdef DEEP_USE_SSE4_1
        return SSE_m128i::Equals(a, b).ToBooleanBitMask() != 0b1111;
        #else
        return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
        #endif
    }
    bool operator==(SSE_m128iArg a, SSE_m128iArg b) {
        return !(a != b);
    }

    SSE_m128i& SSE_m128i::operator+= (SSE_m128iArg other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_add_epi32(_internal, other);
        #else
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        #endif
        return *this;
    }

    SSE_m128i operator+ (SSE_m128i a, SSE_m128iArg b) {
        return a += b;
    }

    SSE_m128i& SSE_m128i::operator-= (SSE_m128iArg other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_sub_epi32(_internal, other);
        #else
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        #endif
        return *this;
    }

    SSE_m128i operator- (SSE_m128i a, SSE_m128iArg b) {
        return a -= b;
    }

    SSE_m128i operator- (SSE_m128iArg a) {
        #ifdef DEEP_USE_SSE4_1
        return _mm_sub_epi32(_mm_setzero_si128(), a);
        #else
        // NOTE(randomuserhi): 0.0f - x to stay consistent with vectorised version
        return SSE_m128i{
            0 - a.x,
            0 - a.y,
            0 - a.z,
            0 - a.w
        };
        #endif
    }

    SSE_m128i& SSE_m128i::operator*= (SSE_m128iArg other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_mul_epi32(_internal, other);
        #else
        x *= other.x;
        y *= other.y;
        z *= other.z;
        w *= other.w;
        #endif
        return *this;
    }
    SSE_m128i operator* (SSE_m128i a, SSE_m128iArg b) {
        return a *= b;
    }

    SSE_m128i& SSE_m128i::operator*= (int32 other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_mul_epi32(_internal, _mm_set1_epi32(other));
        #else
        x *= other;
        y *= other;
        z *= other;
        w *= other;
        #endif
        return *this;
    }

    SSE_m128i operator* (SSE_m128i v, int32 a) {
        return v *= a;
    }

    SSE_m128i operator* (int32 a, SSE_m128iArg v) {
        #ifdef DEEP_USE_SSE4_1
        return _mm_mul_epi32(_mm_set1_epi32(a), v);
        #else
        return SSE_m128i{
            a * v.x,
            a * v.y,
            a * v.z,
            a * v.w
        };
        #endif
    }

    SSE_m128i& SSE_m128i::operator/= (SSE_m128iArg other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_div_epi32(_internal, other);
        #else
        x /= other.x;
        y /= other.y;
        z /= other.z;
        w /= other.w;
        #endif
        return *this;
    }
    SSE_m128i operator/ (SSE_m128i a, SSE_m128iArg b) {
        return a /= b;
    }

    SSE_m128i& SSE_m128i::operator/= (int32 other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_div_epi32(_internal, _mm_set1_epi32(other));
        #else
        x /= other;
        y /= other;
        z /= other;
        w /= other;
        #endif
        return *this;
    }

    SSE_m128i operator/ (SSE_m128i v, int32 a) {
        return v /= a;
    }

    SSE_m128i operator/ (int32 a, SSE_m128iArg v) {
        #ifdef DEEP_USE_SSE4_1
        return _mm_div_epi32(_mm_set1_epi32(a), v);
        #else
        return SSE_m128i{
            a / v.x,
            a / v.y,
            a / v.z,
            a / v.w
        };
        #endif
    }
}