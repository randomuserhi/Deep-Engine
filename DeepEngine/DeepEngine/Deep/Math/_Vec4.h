#pragma once

#include "./_MathTypes.h"
#include <type_traits>

namespace Deep {
    struct [[nodiscard]] alignas(DEEP_VEC_ALIGNMENT) Vec4 {
        #if defined(DEEP_USE_SSE)
        using Type = __m128;
        #else
        using Type = struct {
            float32 values[4];
        };
        #endif

        Vec4() = default;

        Deep_Inline Vec4& Normalize();
        Deep_Inline [[nodiscard]] Vec4 normalized() const;

        Deep_Inline [[nodiscard]] float32 sqrdMagnitude() const;
        Deep_Inline [[nodiscard]] float32 magnitude() const;

        Deep_Inline Vec4& operator+= (const Vec4& other);
        Deep_Inline Vec4& operator-= (const Vec4& other);
        Deep_Inline Vec4& operator*= (const Vec4& other);
        Deep_Inline Vec4& operator/= (const Vec4& other);
        Deep_Inline Vec4& operator*= (float32 other);
        Deep_Inline Vec4& operator/= (float32 other);

        static Deep_Inline float32 Dot(const Vec4& a, const Vec4& b);

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

    static_assert(std::is_trivial<Vec4>(), "Is supposed to be a trivial type!");

    Deep_Inline bool operator!=(const Vec4& a, const Vec4& b);
    Deep_Inline bool operator==(const Vec4& a, const Vec4& b);

    Deep_Inline Vec4 operator+ (Vec4 a, const Vec4& b);
    Deep_Inline Vec4 operator- (Vec4 a, const Vec4& b);
    Deep_Inline Vec4 operator- (Vec4 a);
    Deep_Inline Vec4 operator* (Vec4 v, const float32 a);
    Deep_Inline Vec4 operator* (float32 a, Vec4 v);
    Deep_Inline Vec4 operator/ (Vec4 v, const float32 a);
    Deep_Inline Vec4 operator* (Vec4 a, const Vec4& b);
    Deep_Inline Vec4 operator/ (Vec4 a, const Vec4& b);

    Deep_Inline Vec4 operator* (const Mat4& m, const Vec4& v);
}