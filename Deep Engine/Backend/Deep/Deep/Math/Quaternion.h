#pragma once

#include "./Vec3.h"
#include "./Vec4.h"
#include "./Mat3.h"

namespace Deep {
    struct Quaternion {
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
        Quaternion& operator*= (const float other);
        Quaternion& operator/= (const float other);

        Quaternion() = default;
        Quaternion(float x, float y, float z, float w) 
            : vector{x, y, z, w} {};
        Quaternion(Vec3 axis, float angle);

        union {
            float values[4];
            struct {
                float x;
                float y;
                float z;
                float w;
            };
            Vec4 vector;
        };
    };

    Quaternion operator+ (Quaternion a, const Quaternion& b);
    Quaternion operator- (Quaternion a, const Quaternion& b);
    Quaternion operator* (Quaternion v, const float a);
    Quaternion operator* (const float a, Quaternion v);
    Quaternion operator* (Quaternion a, const Quaternion& b);

    Vec3 operator* (const Quaternion& rot, Vec3 v);
}