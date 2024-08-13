#pragma once

#include "../Math.h"

namespace Deep {
    SSE_mm128i::SSE_mm128i(int32 x, int32 y, int32 z, int32 w) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_set_epi32(w, z, y, x);
        #else
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
        #endif
    }
    SSE_mm128i::SSE_mm128i(Type _internal) : _internal(_internal) {
    }

    SSE_mm128i::operator Type() const {
        return _internal;
    }

    SSE_mm128 SSE_mm128i::ToFloat() const {
        #ifdef DEEP_USE_SSE
        return _mm_cvtepi32_ps(_internal);
        #else
        return SSE_mm128{
            static_cast<float32>(x),
            static_cast<float32>(y),
            static_cast<float32>(z),
            static_cast<float32>(w)
        };
        #endif
    }
    SSE_mm128 SSE_mm128i::ReinterpretAsFloat() const {
        #ifdef DEEP_USE_SSE
        return _mm_castsi128_ps(_internal);
        #else
        return *reinterpret_cast<const SSE_mm128*>(this);
        #endif
    }

    SSE_mm128i SSE_mm128i::Replicate(int value) {
        #ifdef DEEP_USE_SSE
        return _mm_set1_epi32(value);
        #else
        return SSE_mm128i{
            value,
            value,
            value,
            value
        };
        #endif
    }

    int32 SSE_mm128i::ToBooleanBitMask() const {
        #ifdef DEEP_USE_SSE
        return _mm_movemask_ps(_mm_castsi128_ps(_internal));
        #else
        return (x >> 31) | ((y >> 31) << 1) | ((z >> 31) << 2) | ((w >> 31) << 3);
        #endif
    }

    SSE_mm128i SSE_mm128i::Xor(SSE_mm128i a, SSE_mm128i b) {
        #ifdef DEEP_USE_SSE
        return _mm_xor_si128(a, b);
        #else
        return SSE_mm128i{
            a.x ^ b.x,
            a.y ^ b.y,
            a.z ^ b.z,
            a.w ^ b.w
        };
        #endif
    }

    SSE_mm128i SSE_mm128i::And(SSE_mm128i a, SSE_mm128i b) {
        #ifdef DEEP_USE_SSE
        return _mm_and_si128(a, b);
        #else
        return SSE_mm128i{
            a.x & b.x,
            a.y & b.y,
            a.z & b.z,
            a.w & b.w
        };
        #endif
    }

    SSE_mm128i SSE_mm128i::Equals(SSE_mm128i a, SSE_mm128i b) {
        #ifdef DEEP_USE_SSE
        return _mm_cmpeq_epi32(a, b);
        #else
        return SSE_mm128i{
            a.x == b.x ? (int32)0xffffffff : 0,
            a.y == b.y ? (int32)0xffffffff : 0,
            a.z == b.z ? (int32)0xffffffff : 0,
            a.w == b.w ? (int32)0xffffffff : 0
        };
        #endif
    }

    template <const uint Count>
    SSE_mm128i SSE_mm128i::LogicalShiftLeft() const {
        static_assert(Count <= 31, "Invalid shift");

        #ifdef DEEP_USE_SSE
        return _mm_slli_epi32(_internal, Count);
        #else
        return SSE_mm128i{
            x << Count,
            y << Count,
            z << Count,
            w << Count
        };
        #endif
    }

    template <const uint Count>
    SSE_mm128i SSE_mm128i::LogicalShiftRight() const {
        static_assert(Count <= 31, "Invalid shift");

        #ifdef DEEP_USE_SSE
        return _mm_srli_epi32(_internal, Count);
        #else
        return SSE_mm128i{
            x >> Count,
            y >> Count,
            z >> Count,
            w >> Count
        };
        #endif
    }

    SSE_mm128i SSE_mm128i::Select(SSE_mm128i a, SSE_mm128i b, SSE_mm128i control) {
        #ifdef DEEP_USE_SSE4_1
        return _mm_castps_si128(_mm_blendv_ps(_mm_castsi128_ps(a), _mm_castsi128_ps(b), _mm_castsi128_ps(control)));
        #else
        return SSE_mm128i{
            control.x ? b.x : a.x,
            control.y ? b.y : a.y,
            control.z ? b.z : a.z,
            control.w ? b.w : a.w
        };
        #endif
    }
}