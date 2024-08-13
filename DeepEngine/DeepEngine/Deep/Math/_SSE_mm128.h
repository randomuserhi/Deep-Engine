#pragma once

#include "./_MathTypes.h"
#include <type_traits>

namespace Deep {
    struct [[nodiscard]] alignas(DEEP_VEC_ALIGNMENT) SSE_mm128 {
        #if defined(DEEP_USE_SSE)
        using Type = __m128;
        #else
        using Type = struct {
            uint32 values[4];
        };
        #endif

        SSE_mm128() = default;
        SSE_mm128(const SSE_mm128& other) = default;
        SSE_mm128& operator= (const SSE_mm128& other) = default;
        Deep_Inline SSE_mm128(float32 x, float32 y, float32 z, float32 w);
        Deep_Inline SSE_mm128(Type _internal);

        Deep_Inline operator Type() const;

        Deep_Inline SSE_mm128i ToInt() const;
        Deep_Inline SSE_mm128i ReinterpretAsInt() const;

        static Deep_Inline SSE_mm128 Replicate(float32 value);
        static Deep_Inline SSE_mm128 And(SSE_mm128 a, SSE_mm128 b);
        static Deep_Inline SSE_mm128 Xor(SSE_mm128 a, SSE_mm128 b);
        static Deep_Inline SSE_mm128i Equals(SSE_mm128 a, SSE_mm128 b);

        static Deep_Inline SSE_mm128 Select(SSE_mm128 a, SSE_mm128 b, SSE_mm128i control);

        Deep_Inline SSE_mm128& operator+= (const SSE_mm128& other);
        Deep_Inline SSE_mm128& operator-= (const SSE_mm128& other);
        Deep_Inline SSE_mm128& operator*= (const SSE_mm128& other);
        Deep_Inline SSE_mm128& operator/= (const SSE_mm128& other);
        Deep_Inline SSE_mm128& operator*= (float32 other);
        Deep_Inline SSE_mm128& operator/= (float32 other);

        friend Deep_Inline bool operator!=(const SSE_mm128& a, const SSE_mm128& b);
        friend Deep_Inline bool operator==(const SSE_mm128& a, const SSE_mm128& b);

        friend Deep_Inline SSE_mm128 operator+ (SSE_mm128 a, const SSE_mm128& b);
        friend Deep_Inline SSE_mm128 operator- (SSE_mm128 a, const SSE_mm128& b);
        friend Deep_Inline SSE_mm128 operator- (SSE_mm128 a);
        friend Deep_Inline SSE_mm128 operator* (SSE_mm128 a, const SSE_mm128& b);
        friend Deep_Inline SSE_mm128 operator/ (SSE_mm128 a, const SSE_mm128& b);
        friend Deep_Inline SSE_mm128 operator* (SSE_mm128 v, const float32 a);
        friend Deep_Inline SSE_mm128 operator* (float32 a, SSE_mm128 v);
        friend Deep_Inline SSE_mm128 operator/ (SSE_mm128 v, const float32 a);

        inline void SinCos(SSE_mm128& sin, SSE_mm128& cos);

        union {
            Type _internal;
            float32 values[4];
            struct {
                float32 x;
                float32 y;
                float32 z;
                float32 w;
            };
        };
    };

    static_assert(std::is_trivial<SSE_mm128>(), "Is supposed to be a trivial type!");
}