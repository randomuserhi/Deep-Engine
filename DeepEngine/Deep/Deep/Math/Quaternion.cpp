#include "./Quaternion.h"

namespace Deep {
    Quaternion& Quaternion::Normalize() {
        float32 length = Deep::Sqrt(x * x + y * y + z * z + w * w);
        x /= length;
        y /= length;
        z /= length;
        w /= length;
        return *this;
    }

    Quaternion& Quaternion::Inverse() {
        x *= -1;
        y *= -1;
        z *= -1;
        return *this;
    }

    Quaternion::Quaternion(Vec3 axis, float32 angle) {
        axis.Normalize(); // Must be normalized for the below equation

        /**
         * cos(a) + sin(a)(xi, yj, zk)
         * cos(a) + sin(a)xi + sin(a)yj + sin(a)zk
         */
        angle /= 2.0f;
        float32 sin = std::sin(angle);
        x = axis.x * sin;
        y = axis.y * sin;
        z = axis.z * sin;
        w = std::cos(angle);

        Normalize();
    }

    Quaternion& Quaternion::operator+= (const Quaternion& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    Quaternion operator+ (Quaternion a, const Quaternion& b) {
        return a += b;
    }

    Quaternion& Quaternion::operator-= (const Quaternion& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    Quaternion operator- (Quaternion a, const Quaternion& b) {
        return a -= b;
    }

    Quaternion& Quaternion::operator*= (const float32 other) {
        x *= other;
        y *= other;
        z *= other;
        w *= other;
        return *this;
    }

    Quaternion& Quaternion::operator/= (const float32 other) {
        x /= other;
        y /= other;
        z /= other;
        w /= other;
        return *this;
    }

    Quaternion operator* (Quaternion a, const float32 other) {
        return a *= other;
    }

    Quaternion operator* (const float32 other, Quaternion a) {
        return a *= other;
    }

    Quaternion operator/ (Quaternion a, const float32 other) {
        return a /= other;
    }

    Quaternion& Quaternion::operator*= (const Quaternion& other) {
        x = z * other.y + y * other.z + w * other.x + x * other.w;
        y = z * other.x + x * other.z + w * other.y + y * other.w;
        z = y * other.x + x * other.y + w * other.z + z * other.w;
        w = w * other.w - x * other.x - y * other.y - z * other.z;
        return *this;
    }

    Quaternion operator* (Quaternion a, const Quaternion& b) {
        return a *= b;
    }

    // TODO(randomuserhi): Can be optimised to not use a `toMat3` call
    Vec3 operator* (const Quaternion& q, Vec3 v) {
        Mat3 m = q.ToMat3();
        return v *= m;
    }

    Mat3 Quaternion::ToMat3() const {
        float32 w2 = w * w;
        float32 x2 = x * x;
        float32 y2 = y * y;
        float32 z2 = z * z;

        //float32 inverse = 1.0f / (w2 + x2 + y2 + z2);
        const float32 inverse = 1.0f; // NOTE(randomuserhi): Assume quaternion is normalized

        x2 *= inverse;
        y2 *= inverse;
        z2 *= inverse;
        w2 *= inverse;

        float32 xy = x * y * inverse;
        float32 zw = w * z * inverse;

        float32 xz = x * z * inverse;
        float32 yw = w * y * inverse;

        float32 yz = y * z * inverse;
        float32 xw = w * x * inverse;

        Mat3 m;
        m.m00 = x2 - y2 - z2 + w2;
        m.m01 = 2.0f * (xy - zw);
        m.m02 = 2.0f * (xz + yw);
        m.m10 = 2.0f * (xy + zw);
        m.m11 = -x2 + y2 - z2 + w2;
        m.m12 = 2.0f * (yz - xw);
        m.m20 = 2.0f * (xz - yw);
        m.m21 = 2.0f * (yz + xw);
        m.m22 = -x2 - y2 + z2 + w2;
        return m;
    }
}