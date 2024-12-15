#pragma once

namespace Deep {
#ifdef DEEP_USE_SSE4_1
    Vec4::Vec4(float32 x, float32 y, float32 z, float32 w) :
        sse_m128(x, y, z, w) {}
#else
    Vec4::Vec4(float32 x, float32 y, float32 z, float32 w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
#endif

    Vec4::Vec4(SSE_m128 sse_m128) :
        sse_m128(sse_m128) {}

    Vec4::Vec4(Vec3Arg v, float32 w) {
#ifdef DEEP_USE_SSE4_1
        sse_m128 = _mm_blend_ps(v.sse_m128, _mm_set1_ps(w), 8);
#else
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
        this->w = w;
#endif
    }

    Vec4& Vec4::Normalize() {
#ifdef DEEP_USE_SSE4_1
        sse_m128 = _mm_div_ps(sse_m128, _mm_sqrt_ps(_mm_dp_ps(sse_m128, sse_m128, 0xff)));
        return *this;
#else
        return *this /= magnitude();
#endif
    }
    Vec4 Vec4::normalized() const {
        Vec4 v = *this;
        return v.Normalize();
    }
    bool Vec4::IsNormalized(float tolerance) const {
        return Deep::Abs(sqrdMagnitude() - 1.0f) <= tolerance;
    }

    float32 Vec4::sqrdMagnitude() const {
#ifdef DEEP_USE_SSE4_1
        return _mm_cvtss_f32(_mm_dp_ps(sse_m128, sse_m128, 0xff));
#else
        // NOTE(randomuserhi): brackets to keep consistent with vectorised version
        return (x * x + y * y) + (z * z + w * w);
#endif
    }
    float32 Vec4::magnitude() const {
#ifdef DEEP_USE_SSE4_1
        return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(sse_m128, sse_m128, 0xff)));
#else
        return Deep::Sqrt(sqrdMagnitude());
#endif
    }

    float32 Vec4::Dot(Vec4Arg a, Vec4Arg b) {
#ifdef DEEP_USE_SSE4_1
        return _mm_cvtss_f32(_mm_dp_ps(a.sse_m128, b.sse_m128, 0xff));
#else
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
#endif
    }

    Deep_Inline bool operator!=(Vec4Arg a, Vec4Arg b) {
        return a.sse_m128 != b.sse_m128;
    }
    Deep_Inline bool operator==(Vec4Arg a, Vec4Arg b) {
        return !(a != b);
    }

    Vec4& Vec4::operator+=(Vec4Arg other) {
        sse_m128 += other.sse_m128;
        return *this;
    }

    Vec4 operator+(Vec4Arg a, Vec4Arg b) {
        Vec4 result;
        result.sse_m128 = a.sse_m128 + b.sse_m128;
        return result;
    }

    Vec4& Vec4::operator-=(Vec4Arg other) {
        sse_m128 -= other.sse_m128;
        return *this;
    }

    Vec4 operator-(Vec4Arg a, Vec4Arg b) {
        Vec4 result;
        result.sse_m128 = a.sse_m128 - b.sse_m128;
        return result;
    }

    Vec4 operator-(Vec4Arg a) {
        Vec4 result;
        result.sse_m128 = -a.sse_m128;
        return result;
    }

    Vec4& Vec4::operator*=(Vec4Arg other) {
        sse_m128 *= other.sse_m128;
        return *this;
    }
    Vec4 operator*(Vec4Arg a, Vec4Arg b) {
        Vec4 result;
        result.sse_m128 = a.sse_m128 * b.sse_m128;
        return result;
    }

    Vec4& Vec4::operator*=(float32 other) {
        sse_m128 *= other;
        return *this;
    }

    Vec4 operator*(Vec4Arg v, float32 a) {
        Vec4 result;
        result.sse_m128 = v.sse_m128 * a;
        return result;
    }

    Vec4 operator*(float32 a, Vec4Arg v) {
        Vec4 result;
        result.sse_m128 = a * v.sse_m128;
        return result;
    }

    Vec4& Vec4::operator/=(Vec4Arg other) {
        sse_m128 /= other.sse_m128;
        return *this;
    }
    Vec4 operator/(Vec4Arg a, Vec4Arg b) {
        Vec4 result;
        result.sse_m128 = a.sse_m128 / b.sse_m128;
        return result;
    }

    Vec4& Vec4::operator/=(float32 other) {
        sse_m128 /= other;
        return *this;
    }

    Vec4 operator/(Vec4Arg v, float32 a) {
        Vec4 result;
        result.sse_m128 = v.sse_m128 / a;
        return result;
    }

    Vec4 operator/(float32 a, Vec4Arg v) {
        Vec4 result;
        result.sse_m128 = a / v.sse_m128;
        return result;
    }
} // namespace Deep