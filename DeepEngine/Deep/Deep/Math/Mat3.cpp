#include "../Math.h"

namespace Deep {
    Mat3& Mat3::Transpose() {
        float32 temp = m10;
        m10 = m01;
        m01 = temp;

        temp = m20;
        m20 = m02;
        m02 = temp;

        temp = m21;
        m21 = m12;
        m12 = temp;

        return *this;
    }

    Mat3& Mat3::FromQuaternion(const Quaternion& q) {
        float32 w2 = q.w * q.w;
        float32 x2 = q.x * q.x;
        float32 y2 = q.y * q.y;
        float32 z2 = q.z * q.z;

        //float32 inverse = 1.0f / (w2 + x2 + y2 + z2);
        const float32 inverse = 1.0f; // NOTE(randomuserhi): Assume quaternion is normalized

        x2 *= inverse;
        y2 *= inverse;
        z2 *= inverse;
        w2 *= inverse;

        float32 xy = q.x * q.y * inverse;
        float32 zw = q.w * q.z * inverse;

        float32 xz = q.x * q.z * inverse;
        float32 yw = q.w * q.y * inverse;

        float32 yz = q.y * q.z * inverse;
        float32 xw = q.w * q.x * inverse;

        m00 = x2 - y2 - z2 + w2;
        m01 = 2.0f * (xy - zw);
        m02 = 2.0f * (xz + yw);
        m10 = 2.0f * (xy + zw);
        m11 = -x2 + y2 - z2 + w2;
        m12 = 2.0f * (yz - xw);
        m20 = 2.0f * (xz - yw);
        m21 = 2.0f * (yz + xw);
        m22 = -x2 - y2 + z2 + w2;
        return *this;
    }
}