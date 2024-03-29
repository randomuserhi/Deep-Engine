#pragma once

#include "../../Deep.h"
#include "./Mat3.h"

namespace Deep {
    Deep_PushPack struct Vec3 {
        Vec3& Normalize();
        Vec3 normalized() const {
            Vec3 v{ x, y, z };
            return v.Normalize();
        }

        Deep_Inline float32 sqrdMagnitude() const {
            return x * x + y * y + z * z;
        }
        Deep_Inline float32 magnitude() const {
            return std::sqrt(sqrdMagnitude());
        }

        Vec3& operator+= (const Vec3& other);
        Vec3& operator-= (const Vec3& other);
        Vec3& operator*= (const float32 other);
        Vec3& operator/= (const float32 other);

        Vec3& operator*= (const Mat3& m);

        // NOTE(randomuserhi): Fairly sure this format of union inside struct is also UB by
        //                     Cpp abstract machine, but luckily compilers support it as an 
        //                     extension
        union {
            float32 val[3];
            Deep_PushPack struct {
                float32 x;
                float32 y;
                float32 z;
            } Deep_PopPack;
        };
    } Deep_PopPack;

    Vec3 operator+ (Vec3 a, const Vec3& b);
    Vec3 operator- (Vec3 a, const Vec3& b);
    Vec3 operator* (Vec3 v, const float32 a);
    Vec3 operator* (const float32 a, Vec3 v);
    float32 operator* (const Vec3& a, const Vec3& b);

    Vec3 operator* (const Vec3& v, const Mat3& m);
}