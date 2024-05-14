#pragma once

#include "../../Deep.h"

namespace Deep {
    struct Quaternion;

    struct Mat3 {
        Mat3& Transpose();
        Mat3 transposed() const {
            Mat3 q{
                m00, m01, m02,
                m10, m11, m12,
                m20, m21, m22
            };
            return q;
        }

        Mat3& FromQuaternion(const Quaternion& quaternion);

        float32 Determinant();
        Mat3& Inverse();
        Mat3 inversed();

        Mat3& operator= (const Mat4& other);
        Mat3& operator*= (const Mat3& other);

        /**
         * (00, 01, 02)
         * (10, 11, 12)
         * (20, 21, 22)
         */
        union {
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
    };

    Mat3 operator* (const Mat3& a, const Mat3& b);
}