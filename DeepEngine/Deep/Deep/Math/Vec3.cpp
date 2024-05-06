#include <cmath>

#include "./Vec3.h"

namespace Deep {
    Vec3& Vec3::Normalize() {
        float32 length = magnitude();
        x /= length;
        y /= length;
        z /= length;
        return *this;
    }

    Vec3& Vec3::operator+= (const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vec3 operator+ (Vec3 a, const Vec3& b) {
        return a += b;
    }

    Vec3& Vec3::operator-= (const Vec3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vec3 operator- (Vec3 a, const Vec3& b) {
        return a -= b;
    }

    Vec3& Vec3::operator*= (const float32 other) {
        x *= other;
        y *= other;
        z *= other;
        return *this;
    }

    Vec3& Vec3::operator/= (const float32 other) {
        x /= other;
        y /= other;
        z /= other;
        return *this;
    }

    Vec3 operator* (Vec3 a, const float32 other) {
        return a *= other;
    }

    Vec3 operator* (const float32 other, Vec3 a) {
        return a *= other;
    }

    Vec3 operator/ (Vec3 a, const float32 other) {
        return a /= other;
    }

    float32 operator* (const Vec3& a, const Vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    Vec3& Vec3::operator*= (const Mat3& m) {
        float32 _x = x;
        float32 _y = y;
        float32 _z = z;
        x = m.m00 * _x + m.m01 * _y + m.m02 * _z;
        y = m.m10 * _x + m.m11 * _y + m.m12 * _z;
        z = m.m20 * _x + m.m21 * _y + m.m22 * _z;
        return *this;
    }

    Vec3 operator* (const Mat3& m, const Vec3& v) {
        Vec3 _v;
        _v.x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z;
        _v.y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z;
        _v.z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z;
        return _v;
    }
}