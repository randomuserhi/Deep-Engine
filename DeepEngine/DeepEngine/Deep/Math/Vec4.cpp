#include "../Math.h"

namespace Deep {
    Vec4& Vec4::Normalize() {
        float32 length = magnitude();
        x /= length;
        y /= length;
        z /= length;
        w /= length;
        return *this;
    }

    bool operator!=(const Vec4& a, const Vec4& b) {
        return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
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

    Vec4 operator* (const Mat3& m, const Vec4& v) {
        Vec4 _v;
        _v.x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z;
        _v.y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z;
        _v.z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z;
        return _v;
    }

    Vec4 operator* (const Mat4& m, const Vec4& v) {
        Vec4 _v;
        _v.x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03 * v.w;
        _v.y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13 * v.w;
        _v.z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23 * v.w;
        _v.w = m.m30 * v.x + m.m31 * v.y + m.m32 * v.z + m.m33 * v.w;
        return _v;
    }
}