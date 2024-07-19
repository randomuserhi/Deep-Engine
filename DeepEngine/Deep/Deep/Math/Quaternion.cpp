#include "../Math.h"

namespace Deep {
    Quaternion& Quaternion::FromMat3(const Mat3& m) {
        // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm

        // assumes the upper 3x3 of m is a pure rotation matrix (i.e, unscaled)

        const float32 trace = m.m00 + m.m11 + m.m22;

        if (trace > 0) {

            const float32 s = 0.5f / Deep::Sqrt(trace + 1.0f);

            w = 0.25f / s;
            x = (m.m21 - m.m12) * s;
            y = (m.m02 - m.m20) * s;
            z = (m.m10 - m.m01) * s;

        } else if (m.m00 > m.m11 && m.m00 > m.m22) {

            const float32 s = 2.0f * Deep::Sqrt(1.0f + m.m00 - m.m11 - m.m22);

            w = (m.m21 - m.m12) / s;
            x = 0.25f * s;
            y = (m.m01 + m.m10) / s;
            z = (m.m02 + m.m20) / s;

        } else if (m.m11 > m.m22) {

            const float32 s = 2.0f * Deep::Sqrt(1.0f + m.m11 - m.m00 - m.m22);

            w = (m.m02 - m.m20) / s;
            x = (m.m01 + m.m10) / s;
            y = 0.25f * s;
            z = (m.m12 + m.m21) / s;

        } else {

            const float32 s = 2.0f * Deep::Sqrt(1.0f + m.m22 - m.m00 - m.m11);

            w = (m.m10 - m.m01) / s;
            x = (m.m02 + m.m20) / s;
            y = (m.m12 + m.m21) / s;
            z = 0.25f * s;

        }

        return *this;
    }

    Quaternion& Quaternion::FromMat4(const Mat4& m) {
        // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm

        // assumes the upper 3x3 of m is a pure rotation matrix (i.e, unscaled)

        const float32 trace = m.m00 + m.m11 + m.m22;

        if (trace > 0) {

            const float32 s = 0.5f / Deep::Sqrt(trace + 1.0f);

            w = 0.25f / s;
            x = (m.m21 - m.m12) * s;
            y = (m.m02 - m.m20) * s;
            z = (m.m10 - m.m01) * s;

        } else if (m.m00 > m.m11 && m.m00 > m.m22) {

            const float32 s = 2.0f * Deep::Sqrt(1.0f + m.m00 - m.m11 - m.m22);

            w = (m.m21 - m.m12) / s;
            x = 0.25f * s;
            y = (m.m01 + m.m10) / s;
            z = (m.m02 + m.m20) / s;

        } else if (m.m11 > m.m22) {

            const float32 s = 2.0f * Deep::Sqrt(1.0f + m.m11 - m.m00 - m.m22);

            w = (m.m02 - m.m20) / s;
            x = (m.m01 + m.m10) / s;
            y = 0.25f * s;
            z = (m.m12 + m.m21) / s;

        } else {

            const float32 s = 2.0f * Deep::Sqrt(1.0f + m.m22 - m.m00 - m.m11);

            w = (m.m10 - m.m01) / s;
            x = (m.m02 + m.m20) / s;
            y = (m.m12 + m.m21) / s;
            z = 0.25f * s;

        }

        return *this;
    }

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