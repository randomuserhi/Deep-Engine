#pragma once

#include "./Mat3.h"

namespace Deep {
    struct Vec3 {
        Vec3& Normalize();
        Vec3 normalized() const {
            Vec3 v{ x, y, z };
            return v.Normalize();
        }

        Vec3& operator+= (const Vec3& other);
        Vec3& operator-= (const Vec3& other);
        Vec3& operator*= (const float other);
        Vec3& operator/= (const float other);

        Vec3& operator*= (const Mat3& m);

        // NOTE(randomuserhi): Fairly sure this format of union inside struct is also UB by
        //                     Cpp abstract machine, but luckily compilers support it as an 
        //                     extension
        union {
            float values[3];
            struct {
                float x;
                float y;
                float z;
            };
        };
    };

    Vec3 operator+ (Vec3 a, const Vec3& b);
    Vec3 operator- (Vec3 a, const Vec3& b);
    Vec3 operator* (Vec3 v, const float a);
    Vec3 operator* (const float a, Vec3 v);
    float operator* (const Vec3& a, const Vec3& b);

    Vec3 operator* (const Vec3& v, const Mat3& m);
}