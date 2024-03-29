#pragma once

#include "../../Deep.h"

namespace Deep {
    Deep_PushPack struct Vec4 {
        Vec4& Normalize();
        Vec4 normalized() const {
            Vec4 v{ x, y, z, w };
            return v.Normalize();
        }

        Deep_Inline float32 sqrdMagnitude() const {
            return x * x + y * y + z * z + w * w;
        }
        Deep_Inline float32 magnitude() const {
            return std::sqrt(sqrdMagnitude());
        }

        Vec4& operator+= (const Vec4& other);
        Vec4& operator-= (const Vec4& other);
        Vec4& operator*= (const float32 other);
        Vec4& operator/= (const float32 other);

        union {
            float32 values[4];
            Deep_PushPack struct {
                float32 x;
                float32 y;
                float32 z;
                float32 w;
            } Deep_PopPack;
        };
    } Deep_PopPack;

    Vec4 operator+ (Vec4 a, const Vec4& b);
    Vec4 operator- (Vec4 a, const Vec4& b);
    Vec4 operator* (Vec4 v, const float32 a);
    Vec4 operator* (const float32 a, Vec4 v);
    float32 operator* (const Vec4& a, const Vec4& b);
}