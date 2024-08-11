#pragma once

#include "../Math.h"
#include <cassert>

namespace Deep {
    Mat3::Mat3(
        float32 m00, float32 m01, float32 m02,
        float32 m10, float32 m11, float32 m12,
        float32 m20, float32 m21, float32 m22
    ) {
        values[0] = m00;
        values[1] = m10;
        values[2] = m20;
        values[3] = m01;
        values[4] = m11;
        values[5] = m21;
        values[6] = m02;
        values[7] = m12;
        values[8] = m22;
    }

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
    Mat3 Mat3::transposed() const {
        Mat3 q{
            m00, m10, m20,
            m01, m11, m21,
            m02, m12, m22,
        };
        return q;
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
        m10 = 2.0f * (xy + zw);
        m20 = 2.0f * (xz - yw);
        m01 = 2.0f * (xy - zw);
        m11 = -x2 + y2 - z2 + w2;
        m21 = 2.0f * (yz + xw);
        m02 = 2.0f * (xz + yw);
        m12 = 2.0f * (yz - xw);
        m22 = -x2 - y2 + z2 + w2;
        return *this;
    }

    float32 Mat3::Determinant() {
        return m00 * m11 * m22 - m00 * m12 * m21 - m01 * m10 * m22 + m01 * m12 * m20 + m02 * m10 * m21 - m02 * m11 * m20;
    }

    Mat3& Mat3::Inverse() {
        float32 t00 = m22 * m11 - m21 * m12;
        float32 t01 = m21 * m02 - m22 * m01;
        float32 t02 = m12 * m01 - m11 * m02;

        float32 det = m00 * t00 + m10 * t01 + m20 * t02;

        assert(det != 0);

        float32 detInv = 1.0f / det;

        Mat3 temp = *this;

        m00 = t00 * detInv;
        m10 = (temp.m20 * temp.m12 - temp.m22 * temp.m10) * detInv;
        m20 = (temp.m21 * temp.m10 - temp.m20 * temp.m11) * detInv;

        m01 = t01 * detInv;
        m11 = (temp.m22 * temp.m00 - temp.m20 * temp.m02) * detInv;
        m21 = (temp.m20 * temp.m01 - temp.m21 * temp.m00) * detInv;

        m02 = t02 * detInv;
        m12 = (temp.m10 * temp.m02 - temp.m12 * temp.m00) * detInv;
        m22 = (temp.m11 * temp.m00 - temp.m10 * temp.m01) * detInv;

        return *this;
    }

    Mat3 Mat3::inversed() const {
        float32 t00 = m22 * m11 - m21 * m12;
        float32 t01 = m21 * m02 - m22 * m01;
        float32 t02 = m12 * m01 - m11 * m02;

        float32 det = m00 * t00 + m10 * t01 + m20 * t02;

        assert(det != 0);

        float32 detInv = 1.0f / det;

        Mat3 mat;

        mat.m00 = t00 * detInv;
        mat.m10 = (m20 * m12 - m22 * m10) * detInv;
        mat.m20 = (m21 * m10 - m20 * m11) * detInv;

        mat.m01 = t01 * detInv;
        mat.m11 = (m22 * m00 - m20 * m02) * detInv;
        mat.m21 = (m20 * m01 - m21 * m00) * detInv;

        mat.m02 = t02 * detInv;
        mat.m12 = (m10 * m02 - m12 * m00) * detInv;
        mat.m22 = (m11 * m00 - m10 * m01) * detInv;

        return mat;
    }

    Mat3& Mat3::operator= (const Mat4& other) {
        m00 = other.m00;
        m10 = other.m10;
        m20 = other.m20;

        m01 = other.m01;
        m11 = other.m11;
        m21 = other.m21;

        m02 = other.m02;
        m12 = other.m12;
        m22 = other.m22;
    }

    Mat3& Mat3::operator*= (const Mat3& other) {
        Mat3 temp = *this;

        m00 = temp.m00 * other.m00 + temp.m01 * other.m10 + temp.m02 * other.m20;
        m10 = temp.m10 * other.m00 + temp.m11 * other.m10 + temp.m12 * other.m20;
        m20 = temp.m20 * other.m00 + temp.m21 * other.m10 + temp.m22 * other.m20;

        m01 = temp.m00 * other.m01 + temp.m01 * other.m11 + temp.m02 * other.m21;
        m11 = temp.m10 * other.m01 + temp.m11 * other.m11 + temp.m12 * other.m21;
        m21 = temp.m20 * other.m01 + temp.m21 * other.m11 + temp.m22 * other.m21;

        m02 = temp.m00 * other.m02 + temp.m01 * other.m12 + temp.m02 * other.m22;
        m12 = temp.m10 * other.m02 + temp.m11 * other.m12 + temp.m12 * other.m22;
        m22 = temp.m20 * other.m02 + temp.m21 * other.m12 + temp.m22 * other.m22;
    }

    Mat3 operator* (const Mat3& a, const Mat3& b) {
        Mat3 c;
        c.m00 = a.m00 * b.m00 + a.m01 * b.m10 + a.m02 * b.m20;
        c.m10 = a.m10 * b.m00 + a.m11 * b.m10 + a.m12 * b.m20;
        c.m20 = a.m20 * b.m00 + a.m21 * b.m10 + a.m22 * b.m20;

        c.m01 = a.m00 * b.m01 + a.m01 * b.m11 + a.m02 * b.m21;
        c.m11 = a.m10 * b.m01 + a.m11 * b.m11 + a.m12 * b.m21;
        c.m21 = a.m20 * b.m01 + a.m21 * b.m11 + a.m22 * b.m21;

        c.m02 = a.m00 * b.m02 + a.m01 * b.m12 + a.m02 * b.m22;
        c.m12 = a.m10 * b.m02 + a.m11 * b.m12 + a.m12 * b.m22;
        c.m22 = a.m20 * b.m02 + a.m21 * b.m12 + a.m22 * b.m22;

        return c;
    }
}