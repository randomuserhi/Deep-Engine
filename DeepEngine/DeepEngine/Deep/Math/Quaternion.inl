#pragma once

#include "../Math.h"

// TODO(randomuserhi): Vectorisation of all methods

namespace Deep {
    Quaternion::Quaternion(float32 x, float32 y, float32 z, float32 w)
        : vec{ x, y, z, w } {
    };
    Quaternion::Quaternion(Vec4 vec)
        : vec{ vec } {
    };

    // TODO(randomuserhi): vectorise
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
        vec.Normalize();
        return *this;
    }
    Quaternion Quaternion::normalized() const {
        Quaternion q{ x, y, z, w };
        return q.Normalize();
    }

    Mat4 Quaternion::ToMat4() const {
        Mat4 m;
        return m.FromQuaternion(*this);
    }

    Quaternion& Quaternion::Conjugate() {
        // https://stackoverflow.com/questions/56992811/is-there-a-way-to-flip-the-sign-bit-of-32-bit-float-with-xor
        #if defined(DEEP_USE_SSE)
        sse_mm128 = _mm_xor_ps(sse_mm128, _mm_castsi128_ps(_mm_set_epi32(0x80000000, 0x80000000, 0x80000000, 0)));
        #else
        x ^= 0x80000000;
        y ^= 0x80000000;
        z ^= 0x80000000;
        w ^= 0;
        #endif
        return *this;
    }
    Quaternion Quaternion::conjugated() const {
        Quaternion q{ x, y, z, w };
        return q.Conjugate();
    }

    Quaternion& Quaternion::Inverse() {
        return Conjugate() /= vec.magnitude();
    }
    Quaternion Quaternion::inversed() const {
        Quaternion q{ x, y, z, w };
        return q.Inverse();
    }

    Quaternion::Quaternion(Vec3 axis, float32 angle) {
        assert(axis.IsNormalized()); // Must be normalized for the below equation

        // { x,y,z } = axis * sin(0.5f * inAngle)
        //   w       = cos(0.5f * inAngle)

        SSE_mm128 s, c;
        SSE_mm128::Replicate(0.5f * angle).SinCos(s, c);
        sse_mm128 = SSE_mm128::Select(axis.sse_mm128 * s, c, SSE_mm128i{ 0, 0, 0, (int32)0xffffffff });
    }

    bool operator!=(const Quaternion& a, const Quaternion& b) {
        return a.vec != b.vec;
    }
    bool operator==(const Quaternion& a, const Quaternion& b) {
        return !(a != b);
    }

    Quaternion& Quaternion::operator+= (const Quaternion& other) {
        vec += other.vec;
        return *this;
    }

    Quaternion operator+ (Quaternion a, const Quaternion& b) {
        return a += b;
    }

    Quaternion& Quaternion::operator-= (const Quaternion& other) {
        vec -= other.vec;
        return *this;
    }

    Quaternion operator- (Quaternion a, const Quaternion& b) {
        return a -= b;
    }

    Quaternion& Quaternion::operator*= (float32 other) {
        vec *= other;
        return *this;
    }

    Quaternion& Quaternion::operator/= (float32 other) {
        vec /= other;
        return *this;
    }

    Quaternion operator* (Quaternion a, float32 other) {
        return a *= other;
    }

    Quaternion operator* (float32 other, Quaternion a) {
        return a *= other;
    }

    Quaternion operator/ (Quaternion a, float32 other) {
        return a /= other;
    }

    Quaternion& Quaternion::operator*= (const Quaternion& other) {
        #ifdef DEEP_USE_SSE4_1
        // Taken from: https://stackoverflow.com/questions/18542894/how-to-multiply-two-quaternions-with-minimal-instructions
        __m128 abcd = sse_mm128;
        __m128 xyzw = other.sse_mm128;

        __m128 t0 = _mm_shuffle_ps(abcd, abcd, _MM_SHUFFLE(3, 3, 3, 3));
        __m128 t1 = _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 3, 0, 1));

        __m128 t3 = _mm_shuffle_ps(abcd, abcd, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 t4 = _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 0, 3, 2));

        __m128 t5 = _mm_shuffle_ps(abcd, abcd, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 t6 = _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 0, 3, 1));

        // [d,d,d,d] * [z,w,x,y] = [dz,dw,dx,dy]
        __m128 m0 = _mm_mul_ps(t0, t1);

        // [a,a,a,a] * [y,x,w,z] = [ay,ax,aw,az]
        __m128 m1 = _mm_mul_ps(t3, t4);

        // [b,b,b,b] * [z,x,w,y] = [bz,bx,bw,by]
        __m128 m2 = _mm_mul_ps(t5, t6);

        // [c,c,c,c] * [w,z,x,y] = [cw,cz,cx,cy]
        __m128 t7 = _mm_shuffle_ps(abcd, abcd, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 t8 = _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 2, 0, 1));
        __m128 m3 = _mm_mul_ps(t7, t8);

        // [dz,dw,dx,dy] + -[ay,ax,aw,az] = [dz+ay,dw-ax,dx+aw,dy-az]
        __m128 e = _mm_addsub_ps(m0, m1);

        // [dx+aw,dz+ay,dy-az,dw-ax]
        e = _mm_shuffle_ps(e, e, _MM_SHUFFLE(1, 3, 0, 2));

        // [dx+aw,dz+ay,dy-az,dw-ax] + -[bz,bx,bw,by] = [dx+aw+bz,dz+ay-bx,dy-az+bw,dw-ax-by]
        e = _mm_addsub_ps(e, m2);

        // [dz+ay-bx,dw-ax-by,dy-az+bw,dx+aw+bz]
        e = _mm_shuffle_ps(e, e, _MM_SHUFFLE(2, 0, 1, 3));

        // [dz+ay-bx,dw-ax-by,dy-az+bw,dx+aw+bz] + -[cw,cz,cx,cy] = [dz+ay-bx+cw,dw-ax-by-cz,dy-az+bw+cx,dx+aw+bz-cy]
        e = _mm_addsub_ps(e, m3);

        // [dw-ax-by-cz,dz+ay-bx+cw,dy-az+bw+cx,dx+aw+bz-cy]
        sse_mm128 = _mm_shuffle_ps(e, e, _MM_SHUFFLE(2, 3, 1, 0));
        #else
        float lx = x;
        float ly = y;
        float lz = z;
        float lw = w;

        float rx = other.x;
        float ry = other.y;
        float rz = other.z;
        float rw = other.w;

        x = lw * rx + lx * rw + ly * rz - lz * ry;
        y = lw * ry - lx * rz + ly * rw + lz * rx;
        z = lw * rz + lx * ry - ly * rx + lz * rw;
        w = lw * rw - lx * rx - ly * ry - lz * rz;
        #endif
        return *this;
    }

    Quaternion operator* (Quaternion a, const Quaternion& b) {
        return a *= b;
    }

    // TODO(randomuserhi): Can be optimised to not use a `toMat4` call
    Vec3 operator* (const Quaternion& q, const Vec3& v) {
        Mat4 m = q.ToMat4();
        return m * v;
    }
}