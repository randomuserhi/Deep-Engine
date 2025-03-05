#pragma once

#include "_MathTypes.h"
#include <type_traits>

namespace Deep {
    // Matrix4x4
    // Notation is column followed by row, so m10 is column 1, row 0.
    //
    // Implementation based on Jolt: https://github.com/jrouwe/JoltPhysics/tree/master/Jolt/Math
    struct [[nodiscard]] alignas(DEEP_VEC_ALIGNMENT) Mat4 {
        // Constructors
        Mat4() = default;
        Mat4(const Mat4& other) = default;
        Mat4& operator=(const Mat4& other) = default;
        Deep_Inline Mat4(SSE_m128 col0, SSE_m128 col1, SSE_m128 col2, SSE_m128 col3);
        Deep_Inline Mat4(                                       //
            float32 m00, float32 m01, float32 m02, float32 m03, //
            float32 m10, float32 m11, float32 m12, float32 m13, //
            float32 m20, float32 m21, float32 m22, float32 m23, //
            float32 m30, float32 m31, float32 m32, float32 m33  //
        );                                                      //
        Deep_Inline Mat4(Vec4 col0, Vec4 col1, Vec4 col2, Vec4 col3);

        Deep_Inline Mat4& Transpose();
        Deep_Inline [[nodiscard]] Mat4 transposed() const;

        Deep_Inline Mat4& Inverse();
        Deep_Inline [[nodiscard]] Mat4 inversed() const;

        // Equality
        friend bool operator!=(Mat4Arg a, Mat4Arg b);
        friend bool operator==(Mat4Arg a, Mat4Arg b);

        // Mul Matrix4x4s
        friend Deep_Inline Mat4 operator*(Mat4Arg a, Mat4Arg b);

        // Multiply Matrix4x4 and Vectors
        friend Deep_Inline Vec3 operator*(Mat4Arg m, Vec3Arg v); // NOTE(randomuserhi): Assumes Vec4 with w = 1
        friend Deep_Inline Vec4 operator*(Mat4Arg m, Vec4Arg v);

        static Deep_Inline [[nodiscard]] Mat4 FromQuaternion(const Quat& quaternion);

        /**
         * (00, 01, 02, 03)
         * (10, 11, 12, 13)
         * (20, 21, 22, 23)
         * (30, 31, 32, 33)
         */
        union {
            SSE_m128 cols[4];
            Vec4 vcols[4];
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
} // namespace Deep