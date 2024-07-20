#pragma once

#include "../../Deep.h"

namespace Deep {
    struct Mat3;
    struct Mat4;

    struct Vec3 {
        Vec3& Normalize();
        [[nodiscard]] Vec3 normalized() const {
            Vec3 v{ x, y, z };
            return v.Normalize();
        }

        [[nodiscard]] Deep_Inline float32 sqrdMagnitude() const {
            return x * x + y * y + z * z;
        }
        [[nodiscard]] Deep_Inline float32 magnitude() const {
            return Deep::Sqrt(sqrdMagnitude());
        }

        Vec3& operator+= (const Vec3& other);
        Vec3& operator-= (const Vec3& other);
        Vec3& operator*= (const float32 other);
        Vec3& operator/= (const float32 other);

        // NOTE(randomuserhi): This is a pre-multiply operation of `Mat3 * Vec3` that is inplace
        Vec3& operator*= (const Mat3& m);

        // NOTE(randomuserhi): Fairly sure this format of union inside struct is also UB by
        //                     Cpp abstract machine, but luckily compilers support it as an 
        //                     extension
        union {
            float32 val[3];
            struct {
                float32 x;
                float32 y;
                float32 z;
            };
        };
    };

    bool operator!= (const Vec3& a, const Vec3& b);
    Deep_Inline bool operator== (const Vec3& a, const Vec3& b) {
        return !(a != b);
    }

    Vec3 operator+ (Vec3 a, const Vec3& b);
    Vec3 operator- (Vec3 a, const Vec3& b);
    Vec3 operator* (Vec3 v, const float32 a);
    Vec3 operator* (const float32 a, Vec3 v);
    float32 operator* (const Vec3& a, const Vec3& b);

    // NOTE(randomuserhi): Assumes Vec4 with w = 1
    Vec3 operator* (const Mat3& m, const Vec3& v);
    Vec3 operator* (const Mat4& m, const Vec3& v);
}