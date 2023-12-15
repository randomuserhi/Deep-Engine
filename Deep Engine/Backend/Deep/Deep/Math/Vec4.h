#pragma once

namespace Deep {
    struct Vec4 {
        Vec4& Normalize();
        Vec4 normalized() const {
            Vec4 v{ x, y, z, w };
            return v.Normalize();
        }

        Vec4& operator+= (const Vec4& other);
        Vec4& operator-= (const Vec4& other);
        Vec4& operator*= (const float other);
        Vec4& operator/= (const float other);

        union {
            float values[4];
            struct {
                float x;
                float y;
                float z;
                float w;
            };
        };
    };

    Vec4 operator+ (Vec4 a, const Vec4& b);
    Vec4 operator- (Vec4 a, const Vec4& b);
    Vec4 operator* (Vec4 v, const float a);
    Vec4 operator* (const float a, Vec4 v);
    float operator* (const Vec4& a, const Vec4& b);
}