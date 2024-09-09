#pragma once

#include "./_MathTypes.h"
#include <type_traits>

namespace Deep {
    // Implementation based on Jolt: https://github.com/jrouwe/JoltPhysics/tree/master/Jolt/Math
    struct [[nodiscard]] alignas(DEEP_VEC_ALIGNMENT) Vec4 {
        // Constructors
        Vec4() = default;
        Vec4(const Vec4& other) = default;
        Vec4& operator= (const Vec4& other) = default;
        Deep_Inline Vec4(float32 x, float32 y, float32 z, float32 w);

        Deep_Inline Vec4& Normalize();
        Deep_Inline [[nodiscard]] Vec4 normalized() const;
        Deep_Inline [[nodiscard]] bool IsNormalized(float tolerance = 1.0e-6f) const;

        Deep_Inline [[nodiscard]] float32 sqrdMagnitude() const;
        Deep_Inline [[nodiscard]] float32 magnitude() const;

        static Deep_Inline [[nodiscard]] float32 Dot(Vec4Arg a, Vec4Arg b);

        // Equality
        friend Deep_Inline bool operator!= (Vec4Arg a, Vec4Arg b);
        friend Deep_Inline bool operator== (Vec4Arg a, Vec4Arg b);

        // Add vectors
        Deep_Inline Vec4& operator+= (Vec4Arg other);
        friend Deep_Inline Vec4 operator+ (Vec4Arg a, Vec4Arg b);

        // Sub vectors
        Deep_Inline Vec4& operator-= (Vec4Arg other);
        friend Deep_Inline Vec4 operator- (Vec4Arg a, Vec4Arg b);
        friend Deep_Inline Vec4 operator- (Vec4Arg a);

        // Mul vectors
        Deep_Inline Vec4& operator*= (Vec4Arg other);
        friend Deep_Inline Vec4 operator* (Vec4Arg a, Vec4Arg b);

        // Div vectors
        Deep_Inline Vec4& operator/= (Vec4Arg other);
        friend Deep_Inline Vec4 operator/ (Vec4Arg a, Vec4Arg b);

        // Mul vector and float
        Deep_Inline Vec4& operator*= (float32 other);
        friend Deep_Inline Vec4 operator* (Vec4Arg v, const float32 a);
        friend Deep_Inline Vec4 operator* (float32 a, Vec4Arg v);

        // Div vector and float
        Deep_Inline Vec4& operator/= (float32 other);
        friend Deep_Inline Vec4 operator/ (Vec4Arg v, const float32 a);
        friend Deep_Inline Vec4 operator/ (const float32 a, Vec4Arg v);

        // Multiply a Matrix4x4 and Vector
        friend Deep_Inline Vec4 operator* (Mat4Arg m, Vec4Arg v);

        union {
            SSE_m128 sse_m128;
            SSE_m128i sse_m128i;
            float32 values[4];
            struct {
                float32 x;
                float32 y;
                float32 z;
                float32 w;
            };
        };

        static const Vec4 zero;
    };

    static_assert(std::is_trivial<Vec4>(), "Is supposed to be a trivial type!");
}