#pragma once

#include "../Math.h"

// TODO(randomuserhi): Vectorisation of all methods

namespace Deep {
    Quat::Quat(float32 x, float32 y, float32 z, float32 w)
        : vec(x, y, z, w) {
    };
    Quat::Quat(SSE_m128 sse_m128)
        : sse_m128(sse_m128) {
    };
    Quat::Quat(Vec4 vec)
        : vec(vec) {
    };

    Quat Quat::FromMat4(Mat4Arg m) {
        // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm

        float trace = m.m00 + m.m11 + m.m22;

        if (trace >= 0.0f) {
            float s = Deep::Sqrt(trace + 1.0f);
            float is = 0.5f / s;
            return Quat(
                (m.m12 - m.m21) * is,
                (m.m20 - m.m02) * is,
                (m.m01 - m.m10) * is,
                0.5f * s
            );
        } else {
            int i = 0;
            if (m.m11 > m.m00) i = 1;
            if (m.m22 > m.cols[i].values[i]) i = 2;

            if (i == 0) {
                float s = sqrt(m.m00 - (m.m11 + m.m22) + 1);
                float is = 0.5f / s;
                return Quat(
                    0.5f * s,
                    (m.m10 + m.m01) * is,
                    (m.m02 + m.m20) * is,
                    (m.m12 - m.m21) * is
                );
            } else if (i == 1) {
                float s = sqrt(m.m11 - (m.m22 + m.m00) + 1);
                float is = 0.5f / s;
                return Quat(
                    (m.m10 + m.m01) * is,
                    0.5f * s,
                    (m.m21 + m.m12) * is,
                    (m.m20 - m.m02) * is
                );
            } else {
                Deep_Assert(i == 2, "Index should be 2.");

                float s = sqrt(m.m22 - (m.m00 + m.m11) + 1);
                float is = 0.5f / s;
                return Quat(
                    (m.m02 + m.m20) * is,
                    (m.m21 + m.m12) * is,
                    0.5f * s,
                    (m.m01 - m.m10) * is
                );
            }
        }
    }

    Quat& Quat::Normalize() {
        vec.Normalize();
        return *this;
    }
    Quat Quat::normalized() const {
        Quat q = *this;
        return q.Normalize();
    }
    bool Quat::IsNormalized(float tolerance) const {
        return vec.IsNormalized(tolerance);
    }

    Mat4 Quat::ToMat4() const {
        Mat4 m;
        return m.FromQuaternion(*this);
    }

    Quat& Quat::Conjugate() {
        // https://stackoverflow.com/questions/56992811/is-there-a-way-to-flip-the-sign-bit-of-32-bit-float-with-xor
        sse_m128 = SSE_m128::Xor(sse_m128, SSE_m128i{ (int32)0x80000000, (int32)0x80000000, (int32)0x80000000, 0 }.ReinterpretAsFloat());
        return *this;
    }
    Quat Quat::conjugated() const {
        Quat q = *this;
        return q.Conjugate();
    }

    Quat& Quat::Inverse() {
        return Conjugate() /= vec.magnitude();
    }
    Quat Quat::inversed() const {
        Quat q = *this;
        return q.Inverse();
    }

    Quat::Quat(Vec3 axis, float32 angle) {
        Deep_Assert(axis.IsNormalized(), "Axis must be normalized.");

        // { x,y,z } = axis * sin(0.5f * inAngle)
        //   w       = cos(0.5f * inAngle)

        SSE_m128 s, c;
        SSE_m128::Replicate(0.5f * angle).SinCos(s, c);
        sse_m128 = SSE_m128::Select(axis.sse_m128 * s, c, SSE_m128i{ 0, 0, 0, (int32)0xffffffff });
    }

    bool operator!=(QuatArg a, QuatArg b) {
        return a.vec != b.vec;
    }
    bool operator==(QuatArg a, QuatArg b) {
        return !(a != b);
    }

    Quat& Quat::operator+= (QuatArg other) {
        vec += other.vec;
        return *this;
    }

    Quat operator+ (QuatArg a, QuatArg b) {
        return a.vec + b.vec;
    }

    Quat& Quat::operator-= (QuatArg other) {
        vec -= other.vec;
        return *this;
    }

    Quat operator- (QuatArg a, QuatArg b) {
        return a.vec - b.vec;
    }

    Quat& Quat::operator*= (float32 other) {
        vec *= other;
        return *this;
    }

    Quat& Quat::operator/= (float32 other) {
        vec /= other;
        return *this;
    }

    Quat operator* (QuatArg q, float32 a) {
        return q.vec * a;
    }

    Quat operator* (float32 a, QuatArg q) {
        return a * q.vec;
    }

    Quat operator/ (QuatArg q, float32 a) {
        return q.vec / a;
    }

    Quat operator/ (float32 a, QuatArg q) {
        return a / q.vec;
    }

    Quat& Quat::operator*= (QuatArg other) {
        #ifdef DEEP_USE_SSE4_1
        // Taken from: http://momchil-velikov.blogspot.nl/2013/10/fast-sse-quternion-multiplication.html
        __m128 abcd = sse_m128;
        __m128 xyzw = other.sse_m128;

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
        sse_m128 = _mm_shuffle_ps(e, e, _MM_SHUFFLE(2, 3, 1, 0));
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

    Quat operator* (Quat a, QuatArg b) {
        return a *= b;
    }

    Vec3 operator* (QuatArg q, Vec3Arg v) {
        Deep_Assert(q.IsNormalized(), "Quaternion must be normalized.");

        // Rotating a vector by a quaternion is done by: p' = q * p * q^-1 (q^-1 = conjugated(q) for a unit quaternion)
        return Vec3((q * Vec4(v, 0.0f) * q.conjugated()).sse_m128);
    }

    Vec3 Quat::InverseRotate(QuatArg q, Vec3Arg v) {
        Deep_Assert(q.IsNormalized(), "Quaternion must be normalized.");

        // Rotating a vector by a quaternion is done by: p' = q * p * q^-1 (q^-1 = conjugated(q) for a unit quaternion)
        return Vec3((q.conjugated() * Vec4(v, 0.0f) * q).sse_m128);
    }
}