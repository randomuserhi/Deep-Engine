#pragma once

#include "../../Deep.h"

namespace Deep {
    struct Mat4 {
        Mat4& Transpose();
        Mat4 transposed() const {
            Mat4 q{
                m00, m01, m02, m03,
                m10, m11, m12, m13,
                m20, m21, m22, m23,
                m30, m31, m32, m33
            };
            return q;
        }

        Mat4& FromQuaternion(const Quaternion& quaternion);

        float32 Determinant();
        Mat4& Inverse();
        Mat4 inversed();

        Mat4& operator= (const Mat3& other);
        Mat4& operator*= (const Mat4& other);

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