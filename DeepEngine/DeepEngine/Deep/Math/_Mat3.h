#pragma once

#include "./_MathTypes.h"
#include <type_traits>

namespace Deep {
    struct [[nodiscard]] alignas(DEEP_VEC_ALIGNMENT) Mat3 {
        Mat3() = default;
        Deep_Inline Mat3(
            float32 m00, float32 m01, float32 m02,
            float32 m10, float32 m11, float32 m12,
            float32 m20, float32 m21, float32 m22
        );

        Deep_Inline Mat3& Transpose();
        Deep_Inline [[nodiscard]] Mat3 transposed() const;

        Deep_Inline Mat3& FromQuaternion(const Quaternion& quaternion);

        Deep_Inline [[nodiscard]] float32 Determinant();
        Deep_Inline Mat3& Inverse();
        Deep_Inline [[nodiscard]] Mat3 inversed() const;

        Deep_Inline Mat3& operator= (const Mat4& other);
        Deep_Inline Mat3& operator*= (const Mat3& other);

        /**
         * (00, 01, 02)
         * (10, 11, 12)
         * (20, 21, 22)
         */
        union {
            Vec3 columns[3];
            float32 values[9];
            // NOTE(randomuserhi): order of values matter for specific memory access patterns
            //                     column-major ordering is used
            struct {
                float32 m00;
                float32 m10;
                float32 m20;
                float32 m01;
                float32 m11;
                float32 m21;
                float32 m02;
                float32 m12;
                float32 m22;
            };
        };

        static const Mat3 identity;
    };

    static_assert(std::is_trivial<Mat3>(), "Is supposed to be a trivial type!");

    Deep_Inline Mat3 operator* (const Mat3& a, const Mat3& b);
}