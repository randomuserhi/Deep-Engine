#pragma once

#include "_MathTypes.h"
#include <type_traits>

namespace Deep {
    // Implementation based on Jolt: https://github.com/jrouwe/JoltPhysics/tree/master/Jolt/Math
    struct [[nodiscard]] alignas(DEEP_VEC_ALIGNMENT) Vec3 {
        // Constructors
        Vec3() = default;
        Vec3(const Vec3& other) = default;
        Vec3& operator=(const Vec3& other) = default;
        Deep_Inline Vec3(float32 x, float32 y, float32 z);
        explicit Deep_Inline Vec3(SSE_m128 sse_m128);
        explicit Deep_Inline Vec3(Vec4Arg v);

        Deep_Inline Vec3& Normalize();
        Deep_Inline [[nodiscard]] Vec3 normalized() const;
        Deep_Inline [[nodiscard]] bool IsNormalized(float tolerance = 1.0e-6f) const;

        Deep_Inline [[nodiscard]] float32 sqrdMagnitude() const;
        Deep_Inline [[nodiscard]] float32 magnitude() const;

        static Deep_Inline [[nodiscard]] float32 Dot(Vec3Arg a, Vec3Arg b);

        // Equality
        friend bool operator!=(Vec3Arg a, Vec3Arg b);
        friend bool operator==(Vec3Arg a, Vec3Arg b);

        // Add vectors
        Deep_Inline Vec3& operator+=(Vec3Arg other);
        friend Deep_Inline Vec3 operator+(Vec3Arg a, Vec3Arg b);

        // Sub vectors
        Deep_Inline Vec3& operator-=(Vec3Arg other);
        friend Deep_Inline Vec3 operator-(Vec3Arg a, Vec3Arg b);
        friend Deep_Inline Vec3 operator-(Vec3Arg a);

        // Mul vectors
        Deep_Inline Vec3& operator*=(Vec3Arg other);
        friend Deep_Inline Vec3 operator*(Vec3Arg a, Vec3Arg b);

        // Div vectors
        Deep_Inline Vec3& operator/=(Vec3Arg other);
        friend Deep_Inline Vec3 operator/(Vec3Arg a, Vec3Arg b);

        // Mul vector and float
        Deep_Inline Vec3& operator*=(float32 other);
        friend Deep_Inline Vec3 operator*(Vec3Arg v, float32 a);
        friend Deep_Inline Vec3 operator*(float32 a, Vec3Arg v);

        // Div vector and float
        Deep_Inline Vec3& operator/=(float32 other);
        friend Deep_Inline Vec3 operator/(Vec3Arg v, float32 a);
        friend Deep_Inline Vec3 operator/(float32 a, Vec3Arg v);

        // Multiply a Matrix4x4 and Vector
        friend Deep_Inline Vec3 operator*(Mat4Arg m, Vec3Arg v); // NOTE(randomuserhi): Assumes Vec4 with w = 1

        // Apply a quaternion to a vector
        friend Deep_Inline Vec3 operator*(QuatArg rot, Vec3Arg v);

        // NOTE(randomuserhi): The underlying type is a Vec4 for vectorisation
        union {
            SSE_m128 sse_m128;
            SSE_m128i sse_m128i;
            float32 val[4];
            struct {
                float32 x;
                float32 y;
                float32 z;
                // float32 w;
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
} // namespace Deep