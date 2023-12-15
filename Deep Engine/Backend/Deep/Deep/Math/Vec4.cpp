#include <cmath>

#include "./Vec4.h"

namespace Deep {
    Vec4& Vec4::Normalize() {
        float length = std::sqrt(x * x + y * y + z * z + w * w);
        x /= length;
        y /= length;
        z /= length;
        w /= length;
        return *this;
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

    Vec4& Vec4::operator*= (const float other) {
        x *= other;
        y *= other;
        z *= other;
        w *= other;
        return *this;
    }

    Vec4& Vec4::operator/= (const float other) {
        x /= other;
        y /= other;
        z /= other;
        w /= other;
        return *this;
    }

    Vec4 operator* (Vec4 a, const float other) {
        return a *= other;
    }

    Vec4 operator* (const float other, Vec4 a) {
        return a *= other;
    }

    Vec4 operator/ (Vec4 a, const float other) {
        return a /= other;
    }

    float operator* (const Vec4& a, const Vec4& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }
}