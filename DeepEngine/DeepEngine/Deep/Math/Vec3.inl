#pragma once

namespace Deep {
#ifdef DEEP_USE_SSE4_1
    Vec3::Vec3(float32 x, float32 y, float32 z) :
        sse_m128(x, y, z, z) {}
#else
    Vec3::Vec3(float32 x, float32 y, float32 z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
#endif

#ifdef DEEP_USE_SSE4_1
    Vec3::Vec3(Vec4Arg v) :
        sse_m128(v.sse_m128) {}
#else
    Vec3::Vec3(Vec4Arg v) {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
    }
#endif

#ifdef DEEP_USE_SSE4_1
    Vec3::Vec3(SSE_m128 sse_m128) :
        sse_m128(sse_m128) {}
#else
    Vec3::Vec3(SSE_m128 sse_m128) {
        this->x = sse_m128.x;
        this->y = sse_m128.y;
        this->z = sse_m128.z;
    }
#endif

    Vec3& Vec3::Normalize() {
#ifdef DEEP_USE_SSE4_1
        sse_m128 = _mm_div_ps(sse_m128, _mm_sqrt_ps(_mm_dp_ps(sse_m128, sse_m128, 0x7f)));
        return *this;
#else
        return *this /= magnitude();
#endif
    }
    Vec3 Vec3::normalized() const {
        Vec3 v = *this;
        return v.Normalize();
    }
    bool Vec3::IsNormalized(float tolerance) const {
        return Deep::Abs(sqrdMagnitude() - 1.0f) <= tolerance;
    }

    float32 Vec3::sqrdMagnitude() const {
#ifdef DEEP_USE_SSE4_1
        return _mm_cvtss_f32(_mm_dp_ps(sse_m128, sse_m128, 0x7f));
        ;
#else
        return x * x + y * y + z * z;
#endif
    }
    float32 Vec3::magnitude() const {
#ifdef DEEP_USE_SSE4_1
        return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(sse_m128, sse_m128, 0x7f)));
#else
        return Deep::Sqrt(sqrdMagnitude());
#endif
    }

    float32 Vec3::Dot(Vec3Arg a, Vec3Arg b) {
#ifdef DEEP_USE_SSE4_1
        return _mm_cvtss_f32(_mm_dp_ps(a.sse_m128, b.sse_m128, 0x7f));
#else
        return a.x * b.x + a.y * b.y + a.z * b.z;
#endif
    }

    Deep_Inline bool operator!=(Vec3Arg a, Vec3Arg b) {
#ifdef DEEP_USE_SSE4_1
        return (SSE_m128::Equals(a.sse_m128, b.sse_m128).ToBooleanBitMask() & 0b111) != 0b111;
#else
        return a.x != b.x || a.y != b.y || a.z != b.z;
#endif
    }
    Deep_Inline bool operator==(Vec3Arg a, Vec3Arg b) {
        return !(a != b);
    }

    Vec3& Vec3::operator+=(Vec3Arg other) {
#ifdef DEEP_USE_SSE4_1
        sse_m128 += other.sse_m128;
#else
        x += other.x;
        y += other.y;
        z += other.z;
#endif
        return *this;
    }

    Vec3 operator+(Vec3Arg a, Vec3Arg b) {
#ifdef DEEP_USE_SSE4_1
        Vec3 result;
        result.sse_m128 = a.sse_m128 + b.sse_m128;
        return result;
#else
        return Vec3{ a.x + b.x, a.y + b.y, a.z + b.z };
#endif
    }

    Vec3& Vec3::operator-=(Vec3Arg other) {
#ifdef DEEP_USE_SSE4_1
        sse_m128 -= other.sse_m128;
#else
        x -= other.x;
        y -= other.y;
        z -= other.z;
#endif
        return *this;
    }

    Vec3 operator-(Vec3Arg a, Vec3Arg b) {
#ifdef DEEP_USE_SSE4_1
        Vec3 result;
        result.sse_m128 = a.sse_m128 - b.sse_m128;
        return result;
#else
        return Vec3{ a.x - b.x, a.y - b.y, a.z - b.z };
#endif
    }

    Vec3 operator-(Vec3Arg a) {
#ifdef DEEP_USE_SSE4_1
        Vec3 result;
        result.sse_m128 = -a.sse_m128;
        return result;
#else
        // NOTE(randomuserhi): 0.0f - x to stay consistent with vectorised version
        return Vec3{ 0.0f - a.x, 0.0f - a.y, 0.0f - a.z };
#endif
    }

    Vec3& Vec3::operator*=(Vec3Arg other) {
#ifdef DEEP_USE_SSE4_1
        sse_m128 *= other.sse_m128;
#else
        x *= other.x;
        y *= other.y;
        z *= other.z;
#endif
        return *this;
    }
    Vec3 operator*(Vec3Arg a, Vec3Arg b) {
#ifdef DEEP_USE_SSE4_1
        Vec3 result;
        result.sse_m128 = a.sse_m128 * b.sse_m128;
        return result;
#else
        return Vec3{ a.x * b.x, a.y * b.y, a.z * b.z };
#endif
    }

    Vec3& Vec3::operator*=(float32 other) {
#ifdef DEEP_USE_SSE4_1
        sse_m128 *= other;
#else
        x *= other;
        y *= other;
        z *= other;
#endif
        return *this;
    }

    Vec3 operator*(Vec3Arg v, float32 a) {
#ifdef DEEP_USE_SSE4_1
        Vec3 result;
        result.sse_m128 = v.sse_m128 * a;
        return result;
#else
        return Vec3{ v.x * a, v.y * a, v.z * a };
#endif
    }

    Vec3 operator*(float32 a, Vec3Arg v) {
#ifdef DEEP_USE_SSE4_1
        Vec3 result;
        result.sse_m128 = a * v.sse_m128;
        return result;
#else
        return Vec3{ a * v.x, a * v.y, a * v.z };
#endif
    }

    Vec3& Vec3::operator/=(Vec3Arg other) {
#ifdef DEEP_USE_SSE4_1
        sse_m128 /= other.sse_m128;
#else
        x /= other.x;
        y /= other.y;
        z /= other.z;
#endif
        return *this;
    }
    Vec3 operator/(Vec3Arg a, Vec3Arg b) {
#ifdef DEEP_USE_SSE4_1
        Vec3 result;
        result.sse_m128 = a.sse_m128 / b.sse_m128;
        return result;
#else
        return Vec3{ a.x / b.x, a.y / b.y, a.z / b.z };
#endif
    }

    Vec3& Vec3::operator/=(float32 other) {
#ifdef DEEP_USE_SSE4_1
        sse_m128 /= other;
#else
        x /= other;
        y /= other;
        z /= other;
#endif
        return *this;
    }

    Vec3 operator/(Vec3Arg v, float32 a) {
#ifdef DEEP_USE_SSE4_1
        Vec3 result;
        result.sse_m128 = v.sse_m128 / a;
        return result;
#else
        return Vec3{ v.x / a, v.y / a, v.z / a };
#endif
    }

    Vec3 operator/(float32 a, Vec3Arg v) {
#ifdef DEEP_USE_SSE4_1
        Vec3 result;
        result.sse_m128 = a / v.sse_m128;
        return result;
#else
        return Vec3{ a / v.x, a / v.y, a / v.z };
#endif
    }
} // namespace Deep