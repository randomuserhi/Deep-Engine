#include <cmath>

#include "./Vec4.h"

namespace Deep {
    Vec4& Vec4::Normalize() {
        float32 length = magnitude();
        x /= length;
        y /= length;
        z /= length;
        w /= length;
        return *this;
    }

    Deep_Inline float32 Vec4::sqrdMagnitude() const {
        return x * x + y * y + z * z + w * w;
    }

    Deep_Inline float32 Vec4::magnitude() const {
        return std::sqrt(sqrdMagnitude());
    }

    Vec4& Vec4::operator+= (const Vec4& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    Vec4 operator+ (Vec4 a, const Vec4& b) {
        return a += b;
    }

    Vec4& Vec4::operator-= (const Vec4& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    Vec4 operator- (Vec4 a, const Vec4& b) {
        return a -= b;
    }

    Vec4& Vec4::operator*= (const float32 other) {
        x *= other;
        y *= other;
        z *= other;
        w *= other;
        return *this;
    }

    Vec4& Vec4::operator/= (const float32 other) {
        x /= other;
        y /= other;
        z /= other;
        w /= other;
        return *this;
    }

    Vec4 operator* (Vec4 a, const float32 other) {
        return a *= other;
    }

    Vec4 operator* (const float32 other, Vec4 a) {
        return a *= other;
    }

    Vec4 operator/ (Vec4 a, const float32 other) {
        return a /= other;
    }

    float32 operator* (const Vec4& a, const Vec4& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }
}