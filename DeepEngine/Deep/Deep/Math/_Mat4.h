#pragma once

#include "../../Deep.h"

namespace Deep {
    struct Mat4 {
        static const Mat4 identity;

        Mat4& Transpose();
        Mat4 transposed() const {
            Mat4 q{
                m00, m10, m20, m30,
                m01, m11, m21, m31,
                m02, m12, m22, m32,
                m03, m13, m23, m33,
            };
            return q;
        }

        Mat4& FromQuaternion(const Quaternion& quaternion);

        float32 Determinant() const;
        Mat4& Inverse();
        Mat4 inversed() const;

        Mat4& Compose(const Vec3& position, const Quaternion& rotation, const Vec3& scale);
        void Decompose(Vec3& position, Quaternion& rotation, Vec3& scale) const;

        Mat4& operator= (const Mat3& other);
        Mat4& operator*= (const Mat4& other);

        Mat4() = default;
        Deep_Inline Mat4(
            float32 m00, float32 m01, float32 m02, float32 m03,
            float32 m10, float32 m11, float32 m12, float32 m13,
            float32 m20, float32 m21, float32 m22, float32 m23,
            float32 m30, float32 m31, float32 m32, float32 m33
        ) {
            values[0] = m00;
            values[1] = m10;
            values[2] = m20;
            values[3] = m30;
            values[4] = m01;
            values[5] = m11;
            values[6] = m21;
            values[7] = m31;
            values[8] = m02;
            values[9] = m12;
            values[10] = m22;
            values[11] = m32;
            values[12] = m03;
            values[13] = m13;
            values[14] = m23;
            values[15] = m33;
        }

        /**
         * (00, 01, 02, 03)
         * (10, 11, 12, 13)
         * (20, 21, 22, 23)
         * (30, 31, 32, 33)
         */
        union {
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
    };

    Mat4 operator* (const Mat4& a, const Mat4& b);
}