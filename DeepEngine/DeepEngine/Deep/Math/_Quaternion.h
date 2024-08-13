/**
 * Quaternion
 */

#pragma once

#include "./_MathTypes.h"
#include <type_traits>

namespace Deep {
    struct [[nodiscard]] alignas(DEEP_VEC_ALIGNMENT) Quat {
        // Constructors
        Quat() = default;
        Quat(const Quat& other) = default;
        Quat& operator= (const Quat& other) = default;
        Deep_Inline Quat(Vec4 vec);
        Deep_Inline Quat(float32 x, float32 y, float32 z, float32 w);
        Deep_Inline Quat(Vec3 axis, float32 angle);

        Deep_Inline Quat& Normalize();
        Deep_Inline [[nodiscard]] Quat normalized() const;
        Deep_Inline [[nodiscard]] bool IsNormalized(float tolerance = 1.0e-6f) const;

        Deep_Inline Quat& Conjugate();
        Deep_Inline [[nodiscard]] Quat conjugated() const;

        Deep_Inline Quat& Inverse();
        Deep_Inline [[nodiscard]] Quat inversed() const;

        Deep_Inline Mat4 ToMat4() const;
        static Deep_Inline Quat FromMat4(Mat4Arg m);

        // Equality
        friend Deep_Inline bool operator!=(QuatArg a, QuatArg b);
        friend Deep_Inline bool operator==(QuatArg a, QuatArg b);

        // Add quaternions
        Deep_Inline Quat& operator+= (QuatArg other);
        friend Deep_Inline Quat operator+ (QuatArg a, QuatArg b);

        // Sub quaternions
        Deep_Inline Quat& operator-= (QuatArg other);
        friend Deep_Inline Quat operator- (QuatArg a, QuatArg b);

        // Mul quaternions
        Deep_Inline Quat& operator*= (QuatArg other);
        friend Deep_Inline Quat operator* (QuatArg a, QuatArg b);

        // Mul quaternion and float
        Deep_Inline Quat& operator*= (float32 other);
        friend Deep_Inline Quat operator* (QuatArg v, float32 a);
        friend Deep_Inline Quat operator* (float32 a, QuatArg v);

        // Div quaternion and float
        Deep_Inline Quat& operator/= (float32 other);
        friend Deep_Inline Quat operator/(QuatArg q, float32 a);
        friend Deep_Inline Quat operator/(float32 a, QuatArg q);

        // Apply a quaternion to a vector
        friend Deep_Inline Vec3 operator* (QuatArg rot, Vec3Arg v);

        union {
            SSE_m128 sse_m128;
            SSE_m128i sse_m128i;
            float32 val[4];
            struct {
                float32 x;
                float32 y;
                float32 z;
                float32 w;
            };
            Vec4 vec;
        };
    };

    static_assert(std::is_trivial<Quat>(), "Is supposed to be a trivial type!");
}