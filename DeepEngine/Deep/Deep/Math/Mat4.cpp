#include "../Math.h"
#include <cassert>

namespace Deep {
    Mat4& Mat4::Transpose() {
        float32 temp = m10;
        m10 = m01;
        m01 = temp;

        temp = m20;
        m20 = m02;
        m02 = temp;

        temp = m21;
        m21 = m12;
        m12 = temp;

        temp = m30;
        m30 = m03;
        m03 = temp;

        temp = m31;
        m31 = m13;
        m13 = temp;

        temp = m32;
        m32 = m23;
        m23 = temp;

        return *this;
    }

    Mat4& Mat4::FromQuaternion(const Quaternion& q) {
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

    float32 Mat4::Determinant() {
        return m30 * (
            +m03 * m12 * m21
            - m02 * m13 * m21
            - m03 * m11 * m22
            + m01 * m13 * m22
            + m02 * m11 * m23
            - m01 * m12 * m23) +
            m31 * (
                +m00 * m12 * m23
                - m00 * m13 * m22
                + m03 * m10 * m22
                - m02 * m10 * m23
                + m02 * m13 * m20
                - m03 * m12 * m20) +
            m32 * (
                +m00 * m13 * m21
                - m00 * m11 * m23
                - m03 * m10 * m21
                + m01 * m10 * m23
                + m03 * m11 * m20
                - m01 * m13 * m20) +
            m33 * (
                -m02 * m11 * m20
                - m00 * m12 * m21
                + m00 * m11 * m22
                + m02 * m10 * m21
                - m01 * m10 * m22
                + m01 * m12 * m20);
    }

    Mat4& Mat4::Inverse() {
        float32 t00 = m12 * m23 * m31 - m13 * m22 * m31 + m13 * m21 * m32 - m11 * m23 * m32 - m12 * m21 * m33 + m11 * m22 * m33;
        float32 t01 = m03 * m22 * m31 - m02 * m23 * m31 - m03 * m21 * m32 + m01 * m23 * m32 + m02 * m21 * m33 - m01 * m22 * m33;
        float32 t02 = m02 * m13 * m31 - m03 * m12 * m31 + m03 * m11 * m32 - m01 * m13 * m32 - m02 * m11 * m33 + m01 * m12 * m33;
        float32 t03 = m03 * m12 * m21 - m02 * m13 * m21 - m03 * m11 * m22 + m01 * m13 * m22 + m02 * m11 * m23 - m01 * m12 * m23;

        float32 det = m00 * t00 + m10 * t01 + m20 * t02 + m30 * t03;

        assert(det != 0);

        float32 detInv = 1.0f / det;

        Mat4 temp = *this;

        m00 = t00 * detInv;
        m10 = (temp.m13 * temp.m22 * temp.m30 - temp.m12 * temp.m23 * temp.m30 - temp.m13 * temp.m20 * temp.m32 + temp.m10 * temp.m23 * temp.m32 + temp.m12 * temp.m20 * temp.m33 - temp.m10 * temp.m22 * temp.m33) * detInv;
        m20 = (temp.m11 * temp.m23 * temp.m30 - temp.m13 * temp.m21 * temp.m30 + temp.m13 * temp.m20 * temp.m31 - temp.m10 * temp.m23 * temp.m31 - temp.m11 * temp.m20 * temp.m33 + temp.m10 * temp.m21 * temp.m33) * detInv;
        m30 = (temp.m12 * temp.m21 * temp.m30 - temp.m11 * temp.m22 * temp.m30 - temp.m12 * temp.m20 * temp.m31 + temp.m10 * temp.m22 * temp.m31 + temp.m11 * temp.m20 * temp.m32 - temp.m10 * temp.m21 * temp.m32) * detInv;

        m01 = t01 * detInv;
        m11 = (temp.m02 * temp.m23 * temp.m30 - temp.m03 * temp.m22 * temp.m30 + temp.m03 * temp.m20 * temp.m32 - temp.m00 * temp.m23 * temp.m32 - temp.m02 * temp.m20 * temp.m33 + temp.m00 * temp.m22 * temp.m33) * detInv;
        m21 = (temp.m03 * temp.m21 * temp.m30 - temp.m01 * temp.m23 * temp.m30 - temp.m03 * temp.m20 * temp.m31 + temp.m00 * temp.m23 * temp.m31 + temp.m01 * temp.m20 * temp.m33 - temp.m00 * temp.m21 * temp.m33) * detInv;
        m31 = (temp.m01 * temp.m22 * temp.m30 - temp.m02 * temp.m21 * temp.m30 + temp.m02 * temp.m20 * temp.m31 - temp.m00 * temp.m22 * temp.m31 - temp.m01 * temp.m20 * temp.m32 + temp.m00 * temp.m21 * temp.m32) * detInv;

        m02 = t02 * detInv;
        m12 = (temp.m03 * temp.m12 * temp.m30 - temp.m02 * temp.m13 * temp.m30 - temp.m03 * temp.m10 * temp.m32 + temp.m00 * temp.m13 * temp.m32 + temp.m02 * temp.m10 * temp.m33 - temp.m00 * temp.m12 * temp.m33) * detInv;
        m22 = (temp.m01 * temp.m13 * temp.m30 - temp.m03 * temp.m11 * temp.m30 + temp.m03 * temp.m10 * temp.m31 - temp.m00 * temp.m13 * temp.m31 - temp.m01 * temp.m10 * temp.m33 + temp.m00 * temp.m11 * temp.m33) * detInv;
        m32 = (temp.m02 * temp.m11 * temp.m30 - temp.m01 * temp.m12 * temp.m30 - temp.m02 * temp.m10 * temp.m31 + temp.m00 * temp.m12 * temp.m31 + temp.m01 * temp.m10 * temp.m32 - temp.m00 * temp.m11 * temp.m32) * detInv;

        m03 = t03 * detInv;
        m13 = (temp.m02 * temp.m13 * temp.m20 - temp.m03 * temp.m12 * temp.m20 + temp.m03 * temp.m10 * temp.m22 - temp.m00 * temp.m13 * temp.m22 - temp.m02 * temp.m10 * temp.m23 + temp.m00 * temp.m12 * temp.m23) * detInv;
        m23 = (temp.m03 * temp.m11 * temp.m20 - temp.m01 * temp.m13 * temp.m20 - temp.m03 * temp.m10 * temp.m21 + temp.m00 * temp.m13 * temp.m21 + temp.m01 * temp.m10 * temp.m23 - temp.m00 * temp.m11 * temp.m23) * detInv;
        m33 = (temp.m01 * temp.m12 * temp.m20 - temp.m02 * temp.m11 * temp.m20 + temp.m02 * temp.m10 * temp.m21 - temp.m00 * temp.m12 * temp.m21 - temp.m01 * temp.m10 * temp.m22 + temp.m00 * temp.m11 * temp.m22) * detInv;

        return *this;
    }

    Mat4 Mat4::inversed() {
        float32 t00 = m12 * m23 * m31 - m13 * m22 * m31 + m13 * m21 * m32 - m11 * m23 * m32 - m12 * m21 * m33 + m11 * m22 * m33;
        float32 t01 = m03 * m22 * m31 - m02 * m23 * m31 - m03 * m21 * m32 + m01 * m23 * m32 + m02 * m21 * m33 - m01 * m22 * m33;
        float32 t02 = m02 * m13 * m31 - m03 * m12 * m31 + m03 * m11 * m32 - m01 * m13 * m32 - m02 * m11 * m33 + m01 * m12 * m33;
        float32 t03 = m03 * m12 * m21 - m02 * m13 * m21 - m03 * m11 * m22 + m01 * m13 * m22 + m02 * m11 * m23 - m01 * m12 * m23;

        float32 det = m00 * t00 + m10 * t01 + m20 * t02 + m30 * t03;

        assert(det != 0);

        float32 detInv = 1.0f / det;

        Mat4 mat;

        mat.m00 = t00 * detInv;
        mat.m10 = (m13 * m22 * m30 - m12 * m23 * m30 - m13 * m20 * m32 + m10 * m23 * m32 + m12 * m20 * m33 - m10 * m22 * m33) * detInv;
        mat.m20 = (m11 * m23 * m30 - m13 * m21 * m30 + m13 * m20 * m31 - m10 * m23 * m31 - m11 * m20 * m33 + m10 * m21 * m33) * detInv;
        mat.m30 = (m12 * m21 * m30 - m11 * m22 * m30 - m12 * m20 * m31 + m10 * m22 * m31 + m11 * m20 * m32 - m10 * m21 * m32) * detInv;

        mat.m01 = t01 * detInv;
        mat.m11 = (m02 * m23 * m30 - m03 * m22 * m30 + m03 * m20 * m32 - m00 * m23 * m32 - m02 * m20 * m33 + m00 * m22 * m33) * detInv;
        mat.m21 = (m03 * m21 * m30 - m01 * m23 * m30 - m03 * m20 * m31 + m00 * m23 * m31 + m01 * m20 * m33 - m00 * m21 * m33) * detInv;
        mat.m31 = (m01 * m22 * m30 - m02 * m21 * m30 + m02 * m20 * m31 - m00 * m22 * m31 - m01 * m20 * m32 + m00 * m21 * m32) * detInv;

        mat.m02 = t02 * detInv;
        mat.m12 = (m03 * m12 * m30 - m02 * m13 * m30 - m03 * m10 * m32 + m00 * m13 * m32 + m02 * m10 * m33 - m00 * m12 * m33) * detInv;
        mat.m22 = (m01 * m13 * m30 - m03 * m11 * m30 + m03 * m10 * m31 - m00 * m13 * m31 - m01 * m10 * m33 + m00 * m11 * m33) * detInv;
        mat.m32 = (m02 * m11 * m30 - m01 * m12 * m30 - m02 * m10 * m31 + m00 * m12 * m31 + m01 * m10 * m32 - m00 * m11 * m32) * detInv;

        mat.m03 = t03 * detInv;
        mat.m13 = (m02 * m13 * m20 - m03 * m12 * m20 + m03 * m10 * m22 - m00 * m13 * m22 - m02 * m10 * m23 + m00 * m12 * m23) * detInv;
        mat.m23 = (m03 * m11 * m20 - m01 * m13 * m20 - m03 * m10 * m21 + m00 * m13 * m21 + m01 * m10 * m23 - m00 * m11 * m23) * detInv;
        mat.m33 = (m01 * m12 * m20 - m02 * m11 * m20 + m02 * m10 * m21 - m00 * m12 * m21 - m01 * m10 * m22 + m00 * m11 * m22) * detInv;

        return mat;
    }

    Mat4& Mat4::operator= (const Mat3& other) {
        m00 = other.m00;
        m10 = other.m10;
        m20 = other.m20;
        m30 = 0;

        m01 = other.m01;
        m11 = other.m11;
        m21 = other.m21;
        m31 = 0;

        m02 = other.m02;
        m12 = other.m12;
        m22 = other.m22;
        m32 = 0;

        m03 = 0;
        m13 = 0;
        m23 = 0;
        m33 = 1;
    }

    Mat4& Mat4::operator*= (const Mat4& other) {
        Mat4 temp = *this;

        m00 = temp.m00 * other.m00 + temp.m01 * other.m10 + temp.m02 * other.m20 + temp.m03 * other.m30;
        m10 = temp.m10 * other.m00 + temp.m11 * other.m10 + temp.m12 * other.m20 + temp.m13 * other.m30;
        m20 = temp.m20 * other.m00 + temp.m21 * other.m10 + temp.m22 * other.m20 + temp.m23 * other.m30;
        m30 = temp.m30 * other.m00 + temp.m31 * other.m10 + temp.m32 * other.m20 + temp.m33 * other.m30;

        m01 = temp.m00 * other.m01 + temp.m01 * other.m11 + temp.m02 * other.m21 + temp.m03 * other.m31;
        m11 = temp.m10 * other.m01 + temp.m11 * other.m11 + temp.m12 * other.m21 + temp.m13 * other.m31;
        m21 = temp.m20 * other.m01 + temp.m21 * other.m11 + temp.m22 * other.m21 + temp.m23 * other.m31;
        m31 = temp.m30 * other.m01 + temp.m31 * other.m11 + temp.m32 * other.m21 + temp.m33 * other.m31;

        m02 = temp.m00 * other.m02 + temp.m01 * other.m12 + temp.m02 * other.m22 + temp.m03 * other.m32;
        m12 = temp.m10 * other.m02 + temp.m11 * other.m12 + temp.m12 * other.m22 + temp.m13 * other.m32;
        m22 = temp.m20 * other.m02 + temp.m21 * other.m12 + temp.m22 * other.m22 + temp.m23 * other.m32;
        m32 = temp.m30 * other.m02 + temp.m31 * other.m12 + temp.m32 * other.m22 + temp.m33 * other.m32;

        m03 = temp.m00 * other.m03 + temp.m01 * other.m13 + temp.m02 * other.m23 + temp.m03 * other.m33;
        m13 = temp.m10 * other.m03 + temp.m11 * other.m13 + temp.m12 * other.m23 + temp.m13 * other.m33;
        m23 = temp.m20 * other.m03 + temp.m21 * other.m13 + temp.m22 * other.m23 + temp.m23 * other.m33;
        m33 = temp.m30 * other.m03 + temp.m31 * other.m13 + temp.m32 * other.m23 + temp.m33 * other.m33;
    }

    Mat4 operator* (const Mat4& a, const Mat4& b) {
        Mat4 c;
        c.m00 = a.m00 * b.m00 + a.m01 * b.m10 + a.m02 * b.m20 + a.m03 * b.m30;
        c.m10 = a.m10 * b.m00 + a.m11 * b.m10 + a.m12 * b.m20 + a.m13 * b.m30;
        c.m20 = a.m20 * b.m00 + a.m21 * b.m10 + a.m22 * b.m20 + a.m23 * b.m30;
        c.m30 = a.m30 * b.m00 + a.m31 * b.m10 + a.m32 * b.m20 + a.m33 * b.m30;

        c.m01 = a.m00 * b.m01 + a.m01 * b.m11 + a.m02 * b.m21 + a.m03 * b.m31;
        c.m11 = a.m10 * b.m01 + a.m11 * b.m11 + a.m12 * b.m21 + a.m13 * b.m31;
        c.m21 = a.m20 * b.m01 + a.m21 * b.m11 + a.m22 * b.m21 + a.m23 * b.m31;
        c.m31 = a.m30 * b.m01 + a.m31 * b.m11 + a.m32 * b.m21 + a.m33 * b.m31;

        c.m02 = a.m00 * b.m02 + a.m01 * b.m12 + a.m02 * b.m22 + a.m03 * b.m32;
        c.m12 = a.m10 * b.m02 + a.m11 * b.m12 + a.m12 * b.m22 + a.m13 * b.m32;
        c.m22 = a.m20 * b.m02 + a.m21 * b.m12 + a.m22 * b.m22 + a.m23 * b.m32;
        c.m32 = a.m30 * b.m02 + a.m31 * b.m12 + a.m32 * b.m22 + a.m33 * b.m32;

        c.m03 = a.m00 * b.m03 + a.m01 * b.m13 + a.m02 * b.m23 + a.m03 * b.m33;
        c.m13 = a.m10 * b.m03 + a.m11 * b.m13 + a.m12 * b.m23 + a.m13 * b.m33;
        c.m23 = a.m20 * b.m03 + a.m21 * b.m13 + a.m22 * b.m23 + a.m23 * b.m33;
        c.m33 = a.m30 * b.m03 + a.m31 * b.m13 + a.m32 * b.m23 + a.m33 * b.m33;

        return c;
    }
}