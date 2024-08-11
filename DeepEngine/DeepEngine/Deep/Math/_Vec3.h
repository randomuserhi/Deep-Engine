#pragma once

#include "./_MathTypes.h"
#include <type_traits>

namespace Deep {
    struct [[nodiscard]] alignas(DEEP_VEC_ALIGNMENT) Vec3 {
        #if defined(DEEP_USE_SSE)
        using Type = __m128;
        #else
        using Type = Vec4::Type;
        #endif

        Vec3() = default;

        Deep_Inline Vec3& Normalize();
        Deep_Inline [[nodiscard]] Vec3 normalized() const;

        Deep_Inline [[nodiscard]] float32 sqrdMagnitude() const;
        Deep_Inline [[nodiscard]] float32 magnitude() const;

        Deep_Inline Vec3& operator+= (const Vec3& other);
        Deep_Inline Vec3& operator-= (const Vec3& other);
        Deep_Inline Vec3& operator*= (const float32 other);
        Deep_Inline Vec3& operator/= (const float32 other);

        // NOTE(randomuserhi): This is a pre-multiply operation of `Mat3 * Vec3` that is inplace
        Deep_Inline Vec3& operator*= (const Mat3& m);

        // NOTE(randomuserhi): The underlying type is a Vec4 for SIMD instructions
        union {
            Type internal;
            float32 val[4];
            struct {
                float32 x;
                float32 y;
                float32 z;
                float32 _w;
            };
        };
    };

    static_assert(std::is_trivial<Vec3>(), "Is supposed to be a trivial type!");

    Deep_Inline bool operator!= (const Vec3& a, const Vec3& b);
    Deep_Inline bool operator== (const Vec3& a, const Vec3& b);

    Deep_Inline Vec3 operator+ (Vec3 a, const Vec3& b);
    Deep_Inline Vec3 operator- (Vec3 a, const Vec3& b);
    Deep_Inline Vec3 operator* (Vec3 v, const float32 a);
    Deep_Inline Vec3 operator* (const float32 a, Vec3 v);
    Deep_Inline Vec3 operator/ (Vec3 v, const float32 a);
    Deep_Inline float32 operator* (const Vec3& a, const Vec3& b);

    // NOTE(randomuserhi): Assumes Vec4 with w = 1
    Deep_Inline Vec3 operator* (const Mat3& m, const Vec3& v);
    Deep_Inline Vec3 operator* (const Mat4& m, const Vec3& v);
}