/**
 * Quaternion
 */

#pragma once

#include "./_MathTypes.h"
#include <type_traits>

namespace Deep {
    struct [[nodiscard]] alignas(DEEP_VEC_ALIGNMENT) Quaternion {
        Quaternion() = default;
        Deep_Inline Quaternion(Vec4 vec);
        Deep_Inline Quaternion(float32 x, float32 y, float32 z, float32 w);
        Deep_Inline Quaternion(Vec3 axis, float32 angle);

        Deep_Inline Quaternion& Normalize();
        Deep_Inline [[nodiscard]] Quaternion normalized() const;

        Deep_Inline Quaternion& Conjugate();
        Deep_Inline [[nodiscard]] Quaternion conjugated() const;

        Deep_Inline Quaternion& Inverse();
        Deep_Inline [[nodiscard]] Quaternion inversed() const;

        Deep_Inline Mat4 ToMat4() const;

        Deep_Inline Quaternion& FromMat4(const Mat4& m);

        Deep_Inline Quaternion& operator+= (const Quaternion& other);
        Deep_Inline Quaternion& operator-= (const Quaternion& other);
        Deep_Inline Quaternion& operator*= (const Quaternion& other);
        Deep_Inline Quaternion& operator*= (float32 other);
        Deep_Inline Quaternion& operator/= (float32 other);

        friend Deep_Inline bool operator!=(const Quaternion& a, const Quaternion& b);
        friend Deep_Inline bool operator==(const Quaternion& a, const Quaternion& b);

        friend Deep_Inline Quaternion operator+ (Quaternion a, const Quaternion& b);
        friend Deep_Inline Quaternion operator- (Quaternion a, const Quaternion& b);
        friend Deep_Inline Quaternion operator* (Quaternion v, const float32 a);
        friend Deep_Inline Quaternion operator* (float32 a, Quaternion v);
        friend Deep_Inline Quaternion operator* (Quaternion a, const Quaternion& b);
        friend Deep_Inline Quaternion operator/(Quaternion a, float32 other);

        friend Deep_Inline Vec3 operator* (const Quaternion& rot, const Vec3& v);

        union {
            SSE_mm128 sse_mm128;
            SSE_mm128i sse_mm128i;
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

    static_assert(std::is_trivial<Quaternion>(), "Is supposed to be a trivial type!");
}