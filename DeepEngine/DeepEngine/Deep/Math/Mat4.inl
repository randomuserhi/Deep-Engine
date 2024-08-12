#pragma once

#include "../Math.h"

// TODO(randomuserhi): Vectorisation of all methods

namespace Deep {
    Mat4::Mat4(
        float32 m00, float32 m01, float32 m02, float32 m03,
        float32 m10, float32 m11, float32 m12, float32 m13,
        float32 m20, float32 m21, float32 m22, float32 m23,
        float32 m30, float32 m31, float32 m32, float32 m33
    ) {
        values[0] = m00;
        values[1] = m10;
        values[2] = m20;
        values[3] = m30;
        values[4] = m01;
        values[5] = m11;
        values[6] = m21;
        values[7] = m31;
        values[8] = m02;
        values[9] = m12;
        values[10] = m22;
        values[11] = m32;
        values[12] = m03;
        values[13] = m13;
        values[14] = m23;
        values[15] = m33;
    }

    Mat4& Mat4::Compose(const Vec3& position, const Quaternion& rotation, const Vec3& scale) {
        // NOTE(randomuserhi): Assume quaternion is normalized

        const float32 x = rotation.x;
        const float32 y = rotation.y;
        const float32 z = rotation.z;
        const float32 w = rotation.w;

        const float32 x2 = x + x;
        const float32 y2 = y + y;
        const float32 z2 = z + z;

        const float32 xx = x * x2;
        const float32 xy = x * y2;
        const float32 xz = x * z2;

        const float32 yy = y * y2;
        const float32 yz = y * z2;
        const float32 zz = z * z2;

        const float32 wx = w * x2;
        const float32 wy = w * y2;
        const float32 wz = w * z2;

        const float32 sx = scale.x;
        const float32 sy = scale.y;
        const float32 sz = scale.z;

        m00 = (1 - (yy + zz)) * sx;
        m10 = (xy - wz) * sy;
        m20 = (xz + wy) * sz;
        m30 = position.x;

        m01 = (xy + wz) * sx;
        m11 = (1 - (xx + zz)) * sy;
        m21 = (yz - wx) * sz;
        m31 = position.y;

        m02 = (xz - wy) * sx;
        m12 = (yz + wx) * sy;
        m22 = (1 - (xx + yy)) * sz;
        m32 = position.z;

        m03 = 0;
        m13 = 0;
        m23 = 0;
        m33 = 1;

        return *this;
    }

    void Mat4::Decompose(Vec3& position, Quaternion& rotation, Vec3& scale) const {
        float32 sx = Vec3{ m00, m10, m20 }.magnitude();
        const float32 sy = Vec3{ m01, m11, m21 }.magnitude();
        const float32 sz = Vec3{ m02, m12, m22 }.magnitude();

        // if determine is negative, we need to invert one scale
        const float32 det = Determinant();
        if (det < 0) sx = -sx;

        position.x = m30;
        position.y = m31;
        position.z = m32;

        // scale the rotation part
        Mat4 _m1{
            m00, m01, m02, 0,
            m10, m11, m12, 0,
            m20, m21, m22, 0,
            0  , 0  , 0  , 1
        };

        const float32 invSX = 1.0f / sx;
        const float32 invSY = 1.0f / sy;
        const float32 invSZ = 1.0f / sz;

        _m1.m00 *= invSX;
        _m1.m10 *= invSX;
        _m1.m20 *= invSX;

        _m1.m01 *= invSY;
        _m1.m11 *= invSY;
        _m1.m21 *= invSY;

        _m1.m02 *= invSZ;
        _m1.m12 *= invSZ;
        _m1.m22 *= invSZ;

        rotation.FromMat4(_m1);

        scale.x = sx;
        scale.y = sy;
        scale.z = sz;
    }

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
    Mat4 Mat4::transposed() const {
        Mat4 q{
            m00, m10, m20, m30,
            m01, m11, m21, m31,
            m02, m12, m22, m32,
            m03, m13, m23, m33,
        };
        return q;
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

        const float32 xy = q.x * q.y * inverse;
        const float32 zw = q.w * q.z * inverse;

        const float32 xz = q.x * q.z * inverse;
        const float32 yw = q.w * q.y * inverse;

        const float32 yz = q.y * q.z * inverse;
        const float32 xw = q.w * q.x * inverse;

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

    float32 Mat4::Determinant() const {
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

    Mat4 Mat4::inversed() const {
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