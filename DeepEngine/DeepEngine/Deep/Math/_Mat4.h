#pragma once

#include "./_MathTypes.h"
#include <type_traits>

namespace Deep {
    struct [[nodiscard]] alignas(DEEP_VEC_ALIGNMENT) Mat4 {
        Mat4() = default;
        Deep_Inline Mat4(
            float32 m00, float32 m01, float32 m02, float32 m03,
            float32 m10, float32 m11, float32 m12, float32 m13,
            float32 m20, float32 m21, float32 m22, float32 m23,
            float32 m30, float32 m31, float32 m32, float32 m33
        );

        Deep_Inline Mat4& Transpose();
        Deep_Inline [[nodiscard]] Mat4 transposed() const;

        Deep_Inline Mat4& FromQuaternion(const Quaternion& quaternion);

        Deep_Inline [[nodiscard]] float32 Determinant() const;
        Deep_Inline Mat4& Inverse();
        Deep_Inline [[nodiscard]] Mat4 inversed() const;

        Deep_Inline Mat4& Compose(const Vec3& position, const Quaternion& rotation, const Vec3& scale);
        Deep_Inline void Decompose(Vec3& position, Quaternion& rotation, Vec3& scale) const;

        Deep_Inline Mat4& operator*= (const Mat4& other);

        /**
         * (00, 01, 02, 03)
         * (10, 11, 12, 13)
         * (20, 21, 22, 23)
         * (30, 31, 32, 33)
         */
        union {
            Vec4 columns[4];
            float32 values[16];
            // NOTE(randomuserhi): order of values matter for specific memory access patterns
            //                     column-major ordering is used
            struct {
                float32 m00;
                float32 m10;
                float32 m20;
                float32 m30;
                float32 m01;
                float32 m11;
                float32 m21;
                float32 m31;
                float32 m02;
                float32 m12;
                float32 m22;
                float32 m32;
                float32 m03;
                float32 m13;
                float32 m23;
                float32 m33;
            };
        };

        static const Mat4 identity;
    };

    static_assert(std::is_trivial<Mat4>(), "Is supposed to be a trivial type!");

    Deep_Inline Mat4 operator* (const Mat4& a, const Mat4& b);
}