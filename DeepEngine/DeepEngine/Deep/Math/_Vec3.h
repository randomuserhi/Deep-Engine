#pragma once

#include "./_MathTypes.h"
#include <type_traits>

namespace Deep {
    struct [[nodiscard]] alignas(DEEP_VEC_ALIGNMENT) Vec3 {
        Vec3() = default;
        Deep_Inline Vec3(float32 x, float32 y, float32 z);

        Deep_Inline Vec3& Normalize();
        Deep_Inline [[nodiscard]] Vec3 normalized() const;
        Deep_Inline [[nodiscard]] bool IsNormalized(float tolerance = 1.0e-6f) const;

        Deep_Inline [[nodiscard]] float32 sqrdMagnitude() const;
        Deep_Inline [[nodiscard]] float32 magnitude() const;

        Deep_Inline Vec3& operator+= (const Vec3& other);
        Deep_Inline Vec3& operator-= (const Vec3& other);
        Deep_Inline Vec3& operator*= (const Vec3& other);
        Deep_Inline Vec3& operator/= (const Vec3& other);
        Deep_Inline Vec3& operator*= (float32 other);
        Deep_Inline Vec3& operator/= (float32 other);

        friend Deep_Inline bool operator!= (const Vec3& a, const Vec3& b);
        friend Deep_Inline bool operator== (const Vec3& a, const Vec3& b);

        friend Deep_Inline Vec3 operator+ (Vec3 a, const Vec3& b);
        friend Deep_Inline Vec3 operator- (Vec3 a, const Vec3& b);
        friend Deep_Inline Vec3 operator- (Vec3 a);
        friend Deep_Inline Vec3 operator* (Vec3 v, float32 a);
        friend Deep_Inline Vec3 operator* (float32 a, Vec3 v);
        friend Deep_Inline Vec3 operator/ (Vec3 v, float32 a);
        friend Deep_Inline Vec3 operator* (Vec3 a, const Vec3& b);
        friend Deep_Inline Vec3 operator/ (Vec3 a, const Vec3& b);

        // NOTE(randomuserhi): Assumes Vec4 with w = 1
        friend Deep_Inline Vec3 operator* (const Mat4& m, const Vec3& v);

        static Deep_Inline [[nodiscard]] float32 Dot(const Vec3& a, const Vec3& b);

        // NOTE(randomuserhi): The underlying type is a Vec4 for SIMD instructions
        union {
            SSE_mm128 sse_mm128;
            SSE_mm128i sse_mm128i;
            float32 val[4];
            struct {
                float32 x;
                float32 y;
                float32 z;
                //float32 w;
            };
        };

        static const Vec3 zero;

        static const Vec3 up;
        static const Vec3 down;
        static const Vec3 left;
        static const Vec3 right;
        static const Vec3 forwards;
        static const Vec3 backwards;
    };

    static_assert(std::is_trivial<Vec3>(), "Is supposed to be a trivial type!");
}