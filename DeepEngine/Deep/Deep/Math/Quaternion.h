/**
 * Quaternion
 */

#pragma once

#include "../../Deep.h"
#include "./Mat3.h"
#include "./Vec3.h"
#include "./Vec4.h"

namespace Deep {
    Deep_PushPack struct Quaternion {
        Quaternion& Normalize();
        Quaternion normalized() const {
            Quaternion q{ x, y, z, w };
            return q.Normalize();
        }

        Quaternion& Inverse();
        Quaternion inversed() const {
            Quaternion q{ x, y, z, w };
            return q.Inverse();
        }

        Mat3 toMat3() const;

        Quaternion& operator+= (const Quaternion& other);
        Quaternion& operator-= (const Quaternion& other);
        Quaternion& operator*= (const Quaternion& other);
        Quaternion& operator*= (const float32 other);
        Quaternion& operator/= (const float32 other);

        Quaternion() = default;
        Quaternion(float32 x, float32 y, float32 z, float32 w)
            : vec{ x, y, z, w } {
        };
        Quaternion(Vec3 axis, float32 angle);

        union {
            float32 val[4];
            Deep_PushPack struct {
                float32 x;
                float32 y;
                float32 z;
                float32 w;
            } Deep_PopPack;
            Vec4 vec;
        };
    } Deep_PopPack;

    Quaternion operator+ (Quaternion a, const Quaternion& b);
    Quaternion operator- (Quaternion a, const Quaternion& b);
    Quaternion operator* (Quaternion v, const float32 a);
    Quaternion operator* (const float32 a, Quaternion v);
    Quaternion operator* (Quaternion a, const Quaternion& b);

    Vec3 operator* (const Quaternion& rot, Vec3 v);
}