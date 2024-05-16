#include "../Math.h"

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

    bool operator!=(const Quaternion& a, const Quaternion& b) {
        return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
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
}