#pragma once

#include "../Math.h"

namespace Deep {
    Mat4::Mat4(SSE_mm128 col0, SSE_mm128 col1, SSE_mm128 col2, SSE_mm128 col3) : cols{ col0, col1, col2, col3 } {
    }
    Mat4::Mat4(
        float32 m00, float32 m01, float32 m02, float32 m03,
        float32 m10, float32 m11, float32 m12, float32 m13,
        float32 m20, float32 m21, float32 m22, float32 m23,
        float32 m30, float32 m31, float32 m32, float32 m33
    ) : cols{
        SSE_mm128(m00, m10, m20, m30),
        SSE_mm128(m01, m11, m21, m31),
        SSE_mm128(m02, m12, m22, m32),
        SSE_mm128(m03, m13, m23, m33)
    } {
    }
    Mat4::Mat4(Vec4 col0, Vec4 col1, Vec4 col2, Vec4 col3) : vcols{ col0, col1, col2, col3 } {
    }

    Mat4& Mat4::Transpose() {
        #ifdef DEEP_USE_SSE
        __m128 tmp1 = _mm_shuffle_ps(cols[0], cols[1], _MM_SHUFFLE(1, 0, 1, 0));
        __m128 tmp3 = _mm_shuffle_ps(cols[0], cols[1], _MM_SHUFFLE(3, 2, 3, 2));
        __m128 tmp2 = _mm_shuffle_ps(cols[2], cols[3], _MM_SHUFFLE(1, 0, 1, 0));
        __m128 tmp4 = _mm_shuffle_ps(cols[2], cols[3], _MM_SHUFFLE(3, 2, 3, 2));

        cols[0] = _mm_shuffle_ps(tmp1, tmp2, _MM_SHUFFLE(2, 0, 2, 0));
        cols[1] = _mm_shuffle_ps(tmp1, tmp2, _MM_SHUFFLE(3, 1, 3, 1));
        cols[2] = _mm_shuffle_ps(tmp3, tmp4, _MM_SHUFFLE(2, 0, 2, 0));
        cols[3] = _mm_shuffle_ps(tmp3, tmp4, _MM_SHUFFLE(3, 1, 3, 1));
        #else
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
        #endif

        return *this;
    }
    Mat4 Mat4::transposed() const {
        #ifdef DEEP_USE_SSE
        __m128 tmp1 = _mm_shuffle_ps(cols[0], cols[1], _MM_SHUFFLE(1, 0, 1, 0));
        __m128 tmp3 = _mm_shuffle_ps(cols[0], cols[1], _MM_SHUFFLE(3, 2, 3, 2));
        __m128 tmp2 = _mm_shuffle_ps(cols[2], cols[3], _MM_SHUFFLE(1, 0, 1, 0));
        __m128 tmp4 = _mm_shuffle_ps(cols[2], cols[3], _MM_SHUFFLE(3, 2, 3, 2));

        return Mat4{
            _mm_shuffle_ps(tmp1, tmp2, _MM_SHUFFLE(2, 0, 2, 0)),
            _mm_shuffle_ps(tmp1, tmp2, _MM_SHUFFLE(3, 1, 3, 1)),
            _mm_shuffle_ps(tmp3, tmp4, _MM_SHUFFLE(2, 0, 2, 0)),
            _mm_shuffle_ps(tmp3, tmp4, _MM_SHUFFLE(3, 1, 3, 1))
        };
        #else
        return Mat4{
            m00, m10, m20, m30,
            m01, m11, m21, m31,
            m02, m12, m22, m32,
            m03, m13, m23, m33,
        };
        #endif
    }

    Mat4 Mat4::FromQuaternion(const Quaternion& q) {
        assert(q.IsNormalized());

        // See: https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation section 'Quaternion-derived rotation matrix'
        #ifdef DEEP_USE_SSE4_1
        __m128 xyzw = q.sse_mm128;
        __m128 two_xyzw = _mm_add_ps(xyzw, xyzw);
        __m128 yzxw = _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 0, 2, 1));
        __m128 two_yzxw = _mm_add_ps(yzxw, yzxw);
        __m128 zxyw = _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 1, 0, 2));
        __m128 two_zxyw = _mm_add_ps(zxyw, zxyw);
        __m128 wwww = _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 3, 3, 3));
        __m128 diagonal = _mm_sub_ps(_mm_sub_ps(_mm_set1_ps(1.0f), _mm_mul_ps(two_yzxw, yzxw)), _mm_mul_ps(two_zxyw, zxyw));	// (1 - 2 y^2 - 2 z^2, 1 - 2 x^2 - 2 z^2, 1 - 2 x^2 - 2 y^2, 1 - 4 w^2)
        __m128 plus = _mm_add_ps(_mm_mul_ps(two_xyzw, zxyw), _mm_mul_ps(two_yzxw, wwww));										// 2 * (xz + yw, xy + zw, yz + xw, ww)
        __m128 minus = _mm_sub_ps(_mm_mul_ps(two_yzxw, xyzw), _mm_mul_ps(two_zxyw, wwww));										// 2 * (xy - zw, yz - xw, xz - yw, 0)

        // Workaround for compiler changing _mm_sub_ps(_mm_mul_ps(...), ...) into a fused multiply sub instruction, resulting in w not being 0
        // There doesn't appear to be a reliable way to turn this off in Clang
        minus = _mm_insert_ps(minus, minus, 0b1000);

        return Mat4{
            _mm_blend_ps(_mm_blend_ps(plus, diagonal, 0b0001), minus, 0b1100),	// (1 - 2 y^2 - 2 z^2, 2 xy + 2 zw, 2 xz - 2 yw, 0)
            _mm_blend_ps(_mm_blend_ps(diagonal, minus, 0b1001), plus, 0b0100),	// (2 xy - 2 zw, 1 - 2 x^2 - 2 z^2, 2 yz + 2 xw, 0)
            _mm_blend_ps(_mm_blend_ps(minus, plus, 0b0001), diagonal, 0b0100),	// (2 xz + 2 yw, 2 yz - 2 xw, 1 - 2 x^2 - 2 y^2, 0)
            _mm_set_ps(1, 0, 0, 0)
        };
        #else
        float x = q.x;
        float y = q.y;
        float z = q.z;
        float w = q.w;

        // Note: Using x + x instead of 2.0f * x to to stay consistent with vectorised version
        float tx = x + x;
        float ty = y + y;
        float tz = z + z;

        float xx = tx * x;
        float yy = ty * y;
        float zz = tz * z;
        float xy = tx * y;
        float xz = tx * z;
        float xw = tx * w;
        float yz = ty * z;
        float yw = ty * w;
        float zw = tz * w;

        // NOTE(randomuserhi): Brackets to stay consistent with vectorised version
        return Mat4{
            SSE_mm128((1.0f - yy) - zz, xy + zw, xz - yw, 0.0f),
            SSE_mm128(xy - zw, (1.0f - zz) - xx, yz + xw, 0.0f),
            SSE_mm128(xz + yw, yz - xw, (1.0f - xx) - yy, 0.0f),
            SSE_mm128(0.0f, 0.0f, 0.0f, 1.0f)
        };
        #endif
    }

    Mat4& Mat4::Inverse() {
        #ifdef DEEP_USE_SSE
        // Algorithm from: http://download.intel.com/design/PentiumIII/sml/24504301.pdf
        // Streaming SIMD Extensions - Inverse of 4x4 Matrix
        // Adapted to load data using _mm_shuffle_ps instead of loading from memory
        // Replaced _mm_rcp_ps with _mm_div_ps for better accuracy
        __m128 tmp1 = _mm_shuffle_ps(cols[0], cols[1], _MM_SHUFFLE(1, 0, 1, 0));
        __m128 row1 = _mm_shuffle_ps(cols[2], cols[3], _MM_SHUFFLE(1, 0, 1, 0));
        __m128 row0 = _mm_shuffle_ps(tmp1, row1, _MM_SHUFFLE(2, 0, 2, 0));
        row1 = _mm_shuffle_ps(row1, tmp1, _MM_SHUFFLE(3, 1, 3, 1));
        tmp1 = _mm_shuffle_ps(cols[0], cols[1], _MM_SHUFFLE(3, 2, 3, 2));
        __m128 row3 = _mm_shuffle_ps(cols[2], cols[3], _MM_SHUFFLE(3, 2, 3, 2));
        __m128 row2 = _mm_shuffle_ps(tmp1, row3, _MM_SHUFFLE(2, 0, 2, 0));
        row3 = _mm_shuffle_ps(row3, tmp1, _MM_SHUFFLE(3, 1, 3, 1));

        tmp1 = _mm_mul_ps(row2, row3);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(2, 3, 0, 1));
        __m128 minor0 = _mm_mul_ps(row1, tmp1);
        __m128 minor1 = _mm_mul_ps(row0, tmp1);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(1, 0, 3, 2));
        minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp1), minor0);
        minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor1);
        minor1 = _mm_shuffle_ps(minor1, minor1, _MM_SHUFFLE(1, 0, 3, 2));

        tmp1 = _mm_mul_ps(row1, row2);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(2, 3, 0, 1));
        minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor0);
        __m128 minor3 = _mm_mul_ps(row0, tmp1);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(1, 0, 3, 2));
        minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp1));
        minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor3);
        minor3 = _mm_shuffle_ps(minor3, minor3, _MM_SHUFFLE(1, 0, 3, 2));

        tmp1 = _mm_mul_ps(_mm_shuffle_ps(row1, row1, _MM_SHUFFLE(1, 0, 3, 2)), row3);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(2, 3, 0, 1));
        row2 = _mm_shuffle_ps(row2, row2, _MM_SHUFFLE(1, 0, 3, 2));
        minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor0);
        __m128 minor2 = _mm_mul_ps(row0, tmp1);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(1, 0, 3, 2));
        minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp1));
        minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor2);
        minor2 = _mm_shuffle_ps(minor2, minor2, _MM_SHUFFLE(1, 0, 3, 2));

        tmp1 = _mm_mul_ps(row0, row1);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(2, 3, 0, 1));
        minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor2);
        minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp1), minor3);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(1, 0, 3, 2));
        minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp1), minor2);
        minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp1));

        tmp1 = _mm_mul_ps(row0, row3);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(2, 3, 0, 1));
        minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp1));
        minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor2);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(1, 0, 3, 2));
        minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor1);
        minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp1));

        tmp1 = _mm_mul_ps(row0, row2);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(2, 3, 0, 1));
        minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor1);
        minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp1));
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(1, 0, 3, 2));
        minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp1));
        minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor3);

        __m128 det = _mm_mul_ps(row0, minor0);
        // NOTE(randomuserhi): Original code did (x + z) + (y + w), changed to (x + y) + (z + w) to match the non vectorised code
        det = _mm_add_ps(_mm_shuffle_ps(det, det, _MM_SHUFFLE(2, 3, 0, 1)), det);
        det = _mm_add_ss(_mm_shuffle_ps(det, det, _MM_SHUFFLE(1, 0, 3, 2)), det);
        det = _mm_div_ss(_mm_set_ss(1.0f), det);
        det = _mm_shuffle_ps(det, det, _MM_SHUFFLE(0, 0, 0, 0));

        cols[0] = _mm_mul_ps(det, minor0);
        cols[1] = _mm_mul_ps(det, minor1);
        cols[2] = _mm_mul_ps(det, minor2);
        cols[3] = _mm_mul_ps(det, minor3);
        #else
        // NOTE(randomuserhi): Translated column-row to row-column notation for ease of reading
        float32 m00 = this->m00, m10 = this->m01, m20 = this->m02, m30 = this->m03;
        float32 m01 = this->m10, m11 = this->m11, m21 = this->m12, m31 = this->m13;
        float32 m02 = this->m20, m12 = this->m21, m22 = this->m22, m32 = this->m23;
        float32 m03 = this->m30, m13 = this->m31, m23 = this->m32, m33 = this->m33;

        float32 m10211120 = m10 * m21 - m11 * m20;
        float32 m10221220 = m10 * m22 - m12 * m20;
        float32 m10231320 = m10 * m23 - m13 * m20;
        float32 m10311130 = m10 * m31 - m11 * m30;
        float32 m10321230 = m10 * m32 - m12 * m30;
        float32 m10331330 = m10 * m33 - m13 * m30;
        float32 m11221221 = m11 * m22 - m12 * m21;
        float32 m11231321 = m11 * m23 - m13 * m21;
        float32 m11321231 = m11 * m32 - m12 * m31;
        float32 m11331331 = m11 * m33 - m13 * m31;
        float32 m12231322 = m12 * m23 - m13 * m22;
        float32 m12331332 = m12 * m33 - m13 * m32;
        float32 m20312130 = m20 * m31 - m21 * m30;
        float32 m20322230 = m20 * m32 - m22 * m30;
        float32 m20332330 = m20 * m33 - m23 * m30;
        float32 m21322231 = m21 * m32 - m22 * m31;
        float32 m21332331 = m21 * m33 - m23 * m31;
        float32 m22332332 = m22 * m33 - m23 * m32;

        cols[0] = { m11 * m22332332 - m12 * m21332331 + m13 * m21322231, -m10 * m22332332 + m12 * m20332330 - m13 * m20322230, m10 * m21332331 - m11 * m20332330 + m13 * m20312130, -m10 * m21322231 + m11 * m20322230 - m12 * m20312130 };
        cols[1] = { -m01 * m22332332 + m02 * m21332331 - m03 * m21322231, m00 * m22332332 - m02 * m20332330 + m03 * m20322230, -m00 * m21332331 + m01 * m20332330 - m03 * m20312130, m00 * m21322231 - m01 * m20322230 + m02 * m20312130 };
        cols[2] = { m01 * m12331332 - m02 * m11331331 + m03 * m11321231, -m00 * m12331332 + m02 * m10331330 - m03 * m10321230, m00 * m11331331 - m01 * m10331330 + m03 * m10311130, -m00 * m11321231 + m01 * m10321230 - m02 * m10311130 };
        cols[3] = { -m01 * m12231322 + m02 * m11231321 - m03 * m11221221, m00 * m12231322 - m02 * m10231320 + m03 * m10221220, -m00 * m11231321 + m01 * m10231320 - m03 * m10211120, m00 * m11221221 - m01 * m10221220 + m02 * m10211120 };

        float det = m00 * cols[0].x + m01 * cols[0].y + m02 * cols[0].z + m03 * cols[0].w;

        cols[0] /= det;
        cols[1] /= det;
        cols[2] /= det;
        cols[3] /= det;
        #endif

        return *this;
    }

    Mat4 Mat4::inversed() const {
        #ifdef DEEP_USE_SSE
        // Algorithm from: http://download.intel.com/design/PentiumIII/sml/24504301.pdf
        // Streaming SIMD Extensions - Inverse of 4x4 Matrix
        // Adapted to load data using _mm_shuffle_ps instead of loading from memory
        // Replaced _mm_rcp_ps with _mm_div_ps for better accuracy
        __m128 tmp1 = _mm_shuffle_ps(cols[0], cols[1], _MM_SHUFFLE(1, 0, 1, 0));
        __m128 row1 = _mm_shuffle_ps(cols[2], cols[3], _MM_SHUFFLE(1, 0, 1, 0));
        __m128 row0 = _mm_shuffle_ps(tmp1, row1, _MM_SHUFFLE(2, 0, 2, 0));
        row1 = _mm_shuffle_ps(row1, tmp1, _MM_SHUFFLE(3, 1, 3, 1));
        tmp1 = _mm_shuffle_ps(cols[0], cols[1], _MM_SHUFFLE(3, 2, 3, 2));
        __m128 row3 = _mm_shuffle_ps(cols[2], cols[3], _MM_SHUFFLE(3, 2, 3, 2));
        __m128 row2 = _mm_shuffle_ps(tmp1, row3, _MM_SHUFFLE(2, 0, 2, 0));
        row3 = _mm_shuffle_ps(row3, tmp1, _MM_SHUFFLE(3, 1, 3, 1));

        tmp1 = _mm_mul_ps(row2, row3);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(2, 3, 0, 1));
        __m128 minor0 = _mm_mul_ps(row1, tmp1);
        __m128 minor1 = _mm_mul_ps(row0, tmp1);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(1, 0, 3, 2));
        minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp1), minor0);
        minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor1);
        minor1 = _mm_shuffle_ps(minor1, minor1, _MM_SHUFFLE(1, 0, 3, 2));

        tmp1 = _mm_mul_ps(row1, row2);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(2, 3, 0, 1));
        minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor0);
        __m128 minor3 = _mm_mul_ps(row0, tmp1);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(1, 0, 3, 2));
        minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp1));
        minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor3);
        minor3 = _mm_shuffle_ps(minor3, minor3, _MM_SHUFFLE(1, 0, 3, 2));

        tmp1 = _mm_mul_ps(_mm_shuffle_ps(row1, row1, _MM_SHUFFLE(1, 0, 3, 2)), row3);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(2, 3, 0, 1));
        row2 = _mm_shuffle_ps(row2, row2, _MM_SHUFFLE(1, 0, 3, 2));
        minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor0);
        __m128 minor2 = _mm_mul_ps(row0, tmp1);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(1, 0, 3, 2));
        minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp1));
        minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor2);
        minor2 = _mm_shuffle_ps(minor2, minor2, _MM_SHUFFLE(1, 0, 3, 2));

        tmp1 = _mm_mul_ps(row0, row1);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(2, 3, 0, 1));
        minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor2);
        minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp1), minor3);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(1, 0, 3, 2));
        minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp1), minor2);
        minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp1));

        tmp1 = _mm_mul_ps(row0, row3);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(2, 3, 0, 1));
        minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp1));
        minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor2);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(1, 0, 3, 2));
        minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor1);
        minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp1));

        tmp1 = _mm_mul_ps(row0, row2);
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(2, 3, 0, 1));
        minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor1);
        minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp1));
        tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(1, 0, 3, 2));
        minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp1));
        minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor3);

        __m128 det = _mm_mul_ps(row0, minor0);
        // NOTE(randomuserhi): Original code did (x + z) + (y + w), changed to (x + y) + (z + w) to match the non vectorised code
        det = _mm_add_ps(_mm_shuffle_ps(det, det, _MM_SHUFFLE(2, 3, 0, 1)), det);
        det = _mm_add_ss(_mm_shuffle_ps(det, det, _MM_SHUFFLE(1, 0, 3, 2)), det);
        det = _mm_div_ss(_mm_set_ss(1.0f), det);
        det = _mm_shuffle_ps(det, det, _MM_SHUFFLE(0, 0, 0, 0));

        return Mat4{
            _mm_mul_ps(det, minor0),
            _mm_mul_ps(det, minor1),
            _mm_mul_ps(det, minor2),
            _mm_mul_ps(det, minor3)
        };
        #else
        // NOTE(randomuserhi): Translated column-row to row-column notation for ease of reading
        float32 m00 = this->m00, m10 = this->m01, m20 = this->m02, m30 = this->m03;
        float32 m01 = this->m10, m11 = this->m11, m21 = this->m12, m31 = this->m13;
        float32 m02 = this->m20, m12 = this->m21, m22 = this->m22, m32 = this->m23;
        float32 m03 = this->m30, m13 = this->m31, m23 = this->m32, m33 = this->m33;

        float32 m10211120 = m10 * m21 - m11 * m20;
        float32 m10221220 = m10 * m22 - m12 * m20;
        float32 m10231320 = m10 * m23 - m13 * m20;
        float32 m10311130 = m10 * m31 - m11 * m30;
        float32 m10321230 = m10 * m32 - m12 * m30;
        float32 m10331330 = m10 * m33 - m13 * m30;
        float32 m11221221 = m11 * m22 - m12 * m21;
        float32 m11231321 = m11 * m23 - m13 * m21;
        float32 m11321231 = m11 * m32 - m12 * m31;
        float32 m11331331 = m11 * m33 - m13 * m31;
        float32 m12231322 = m12 * m23 - m13 * m22;
        float32 m12331332 = m12 * m33 - m13 * m32;
        float32 m20312130 = m20 * m31 - m21 * m30;
        float32 m20322230 = m20 * m32 - m22 * m30;
        float32 m20332330 = m20 * m33 - m23 * m30;
        float32 m21322231 = m21 * m32 - m22 * m31;
        float32 m21332331 = m21 * m33 - m23 * m31;
        float32 m22332332 = m22 * m33 - m23 * m32;

        Mat4 result{
            SSE_mm128{ m11 * m22332332 - m12 * m21332331 + m13 * m21322231, -m10 * m22332332 + m12 * m20332330 - m13 * m20322230, m10 * m21332331 - m11 * m20332330 + m13 * m20312130, -m10 * m21322231 + m11 * m20322230 - m12 * m20312130 },
            SSE_mm128{ -m01 * m22332332 + m02 * m21332331 - m03 * m21322231, m00 * m22332332 - m02 * m20332330 + m03 * m20322230, -m00 * m21332331 + m01 * m20332330 - m03 * m20312130, m00 * m21322231 - m01 * m20322230 + m02 * m20312130 },
            SSE_mm128{ m01 * m12331332 - m02 * m11331331 + m03 * m11321231, -m00 * m12331332 + m02 * m10331330 - m03 * m10321230, m00 * m11331331 - m01 * m10331330 + m03 * m10311130, -m00 * m11321231 + m01 * m10321230 - m02 * m10311130 },
            SSE_mm128{ -m01 * m12231322 + m02 * m11231321 - m03 * m11221221, m00 * m12231322 - m02 * m10231320 + m03 * m10221220, -m00 * m11231321 + m01 * m10231320 - m03 * m10211120, m00 * m11221221 - m01 * m10221220 + m02 * m10211120 }
        };

        float det = m00 * result.cols[0].x + m01 * result.cols[0].y + m02 * result.cols[0].z + m03 * result.cols[0].w;

        result.cols[0] /= det;
        result.cols[1] /= det;
        result.cols[2] /= det;
        result.cols[3] /= det;
        #endif
    }

    Mat4& Mat4::operator*= (const Mat4& other) {
        Mat4 temp = *this;

        #ifdef DEEP_USE_SSE
        cols[0] = _mm_mul_ps(temp.cols[0], _mm_shuffle_ps(other.cols[0], other.cols[0], _MM_SHUFFLE(0, 0, 0, 0)));
        cols[0] = _mm_add_ps(cols[0], _mm_mul_ps(temp.cols[1], _mm_shuffle_ps(other.cols[0], other.cols[0], _MM_SHUFFLE(1, 1, 1, 1))));
        cols[0] = _mm_add_ps(cols[0], _mm_mul_ps(temp.cols[2], _mm_shuffle_ps(other.cols[0], other.cols[0], _MM_SHUFFLE(2, 2, 2, 2))));
        cols[0] = _mm_add_ps(cols[0], _mm_mul_ps(temp.cols[3], _mm_shuffle_ps(other.cols[0], other.cols[0], _MM_SHUFFLE(3, 3, 3, 3))));

        cols[1] = _mm_mul_ps(temp.cols[0], _mm_shuffle_ps(other.cols[1], other.cols[1], _MM_SHUFFLE(0, 0, 0, 0)));
        cols[1] = _mm_add_ps(cols[1], _mm_mul_ps(temp.cols[1], _mm_shuffle_ps(other.cols[1], other.cols[1], _MM_SHUFFLE(1, 1, 1, 1))));
        cols[1] = _mm_add_ps(cols[1], _mm_mul_ps(temp.cols[2], _mm_shuffle_ps(other.cols[1], other.cols[1], _MM_SHUFFLE(2, 2, 2, 2))));
        cols[1] = _mm_add_ps(cols[1], _mm_mul_ps(temp.cols[3], _mm_shuffle_ps(other.cols[1], other.cols[1], _MM_SHUFFLE(3, 3, 3, 3))));

        cols[2] = _mm_mul_ps(temp.cols[0], _mm_shuffle_ps(other.cols[2], other.cols[2], _MM_SHUFFLE(0, 0, 0, 0)));
        cols[2] = _mm_add_ps(cols[2], _mm_mul_ps(temp.cols[1], _mm_shuffle_ps(other.cols[2], other.cols[2], _MM_SHUFFLE(1, 1, 1, 1))));
        cols[2] = _mm_add_ps(cols[2], _mm_mul_ps(temp.cols[2], _mm_shuffle_ps(other.cols[2], other.cols[2], _MM_SHUFFLE(2, 2, 2, 2))));
        cols[2] = _mm_add_ps(cols[2], _mm_mul_ps(temp.cols[3], _mm_shuffle_ps(other.cols[2], other.cols[2], _MM_SHUFFLE(3, 3, 3, 3))));

        cols[3] = _mm_mul_ps(temp.cols[0], _mm_shuffle_ps(other.cols[3], other.cols[3], _MM_SHUFFLE(0, 0, 0, 0)));
        cols[3] = _mm_add_ps(cols[3], _mm_mul_ps(temp.cols[1], _mm_shuffle_ps(other.cols[3], other.cols[3], _MM_SHUFFLE(1, 1, 1, 1))));
        cols[3] = _mm_add_ps(cols[3], _mm_mul_ps(temp.cols[2], _mm_shuffle_ps(other.cols[3], other.cols[3], _MM_SHUFFLE(2, 2, 2, 2))));
        cols[3] = _mm_add_ps(cols[3], _mm_mul_ps(temp.cols[3], _mm_shuffle_ps(other.cols[3], other.cols[3], _MM_SHUFFLE(3, 3, 3, 3))));
        #else
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
        #endif

        return *this;
    }

    Mat4 operator* (const Mat4& a, const Mat4& b) {
        Mat4 c;

        #ifdef DEEP_USE_SSE
        c.cols[0] = _mm_mul_ps(a.cols[0], _mm_shuffle_ps(b.cols[0], b.cols[0], _MM_SHUFFLE(0, 0, 0, 0)));
        c.cols[0] = _mm_add_ps(c.cols[0], _mm_mul_ps(a.cols[1], _mm_shuffle_ps(b.cols[0], b.cols[0], _MM_SHUFFLE(1, 1, 1, 1))));
        c.cols[0] = _mm_add_ps(c.cols[0], _mm_mul_ps(a.cols[2], _mm_shuffle_ps(b.cols[0], b.cols[0], _MM_SHUFFLE(2, 2, 2, 2))));
        c.cols[0] = _mm_add_ps(c.cols[0], _mm_mul_ps(a.cols[3], _mm_shuffle_ps(b.cols[0], b.cols[0], _MM_SHUFFLE(3, 3, 3, 3))));

        c.cols[1] = _mm_mul_ps(a.cols[0], _mm_shuffle_ps(b.cols[1], b.cols[1], _MM_SHUFFLE(0, 0, 0, 0)));
        c.cols[1] = _mm_add_ps(c.cols[1], _mm_mul_ps(a.cols[1], _mm_shuffle_ps(b.cols[1], b.cols[1], _MM_SHUFFLE(1, 1, 1, 1))));
        c.cols[1] = _mm_add_ps(c.cols[1], _mm_mul_ps(a.cols[2], _mm_shuffle_ps(b.cols[1], b.cols[1], _MM_SHUFFLE(2, 2, 2, 2))));
        c.cols[1] = _mm_add_ps(c.cols[1], _mm_mul_ps(a.cols[3], _mm_shuffle_ps(b.cols[1], b.cols[1], _MM_SHUFFLE(3, 3, 3, 3))));

        c.cols[2] = _mm_mul_ps(a.cols[0], _mm_shuffle_ps(b.cols[2], b.cols[2], _MM_SHUFFLE(0, 0, 0, 0)));
        c.cols[2] = _mm_add_ps(c.cols[2], _mm_mul_ps(a.cols[1], _mm_shuffle_ps(b.cols[2], b.cols[2], _MM_SHUFFLE(1, 1, 1, 1))));
        c.cols[2] = _mm_add_ps(c.cols[2], _mm_mul_ps(a.cols[2], _mm_shuffle_ps(b.cols[2], b.cols[2], _MM_SHUFFLE(2, 2, 2, 2))));
        c.cols[2] = _mm_add_ps(c.cols[2], _mm_mul_ps(a.cols[3], _mm_shuffle_ps(b.cols[2], b.cols[2], _MM_SHUFFLE(3, 3, 3, 3))));

        c.cols[3] = _mm_mul_ps(a.cols[0], _mm_shuffle_ps(b.cols[3], b.cols[3], _MM_SHUFFLE(0, 0, 0, 0)));
        c.cols[3] = _mm_add_ps(c.cols[3], _mm_mul_ps(a.cols[1], _mm_shuffle_ps(b.cols[3], b.cols[3], _MM_SHUFFLE(1, 1, 1, 1))));
        c.cols[3] = _mm_add_ps(c.cols[3], _mm_mul_ps(a.cols[2], _mm_shuffle_ps(b.cols[3], b.cols[3], _MM_SHUFFLE(2, 2, 2, 2))));
        c.cols[3] = _mm_add_ps(c.cols[3], _mm_mul_ps(a.cols[3], _mm_shuffle_ps(b.cols[3], b.cols[3], _MM_SHUFFLE(3, 3, 3, 3))));
        #else
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
        #endif

        return c;
    }
}