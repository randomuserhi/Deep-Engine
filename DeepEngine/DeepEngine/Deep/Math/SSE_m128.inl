#pragma once

namespace Deep {
    SSE_m128::SSE_m128(float32 x, float32 y, float32 z, float32 w) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_set_ps(w, z, y, x);
        #else
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
        #endif
    }
    SSE_m128::SSE_m128(Type _internal) : _internal(_internal) {
    }

    SSE_m128::operator Type() const {
        return _internal;
    }

    SSE_m128i SSE_m128::ToInt() const {
        #ifdef DEEP_USE_SSE
        return _mm_cvttps_epi32(_internal);
        #else
        return SSE_m128i{
            static_cast<int32>(x),
            static_cast<int32>(y),
            static_cast<int32>(z),
            static_cast<int32>(w)
        };
        #endif
    }
    SSE_m128i SSE_m128::ReinterpretAsInt() const {
        #ifdef DEEP_USE_SSE
        return _mm_castps_si128(_internal);
        #else
        return *reinterpret_cast<const SSE_m128i*>(this);
        #endif
    }

    SSE_m128 SSE_m128::Replicate(float32 value) {
        #ifdef DEEP_USE_SSE
        return _mm_set1_ps(value);
        #else
        return SSE_m128{
            value,
            value,
            value,
            value
        };
        #endif
    }
    SSE_m128 SSE_m128::And(SSE_m128Arg a, SSE_m128Arg b) {
        #ifdef DEEP_USE_SSE
        return _mm_and_ps(a, b);
        #else
        return SSE_m128i::And(a.ReinterpretAsInt(), b.ReinterpretAsInt()).ReinterpretAsFloat();
        #endif
    }
    SSE_m128 SSE_m128::Xor(SSE_m128Arg a, SSE_m128Arg b) {
        #ifdef DEEP_USE_SSE
        return _mm_xor_ps(a, b);
        #else
        return SSE_m128i::Xor(a.ReinterpretAsInt(), b.ReinterpretAsInt()).ReinterpretAsFloat();
        #endif
    }
    SSE_m128i SSE_m128::Equals(SSE_m128Arg a, SSE_m128Arg b) {
        #ifdef DEEP_USE_SSE
        return _mm_castps_si128(_mm_cmpeq_ps(a, b));
        #else
        return SSE_m128i{
            a.x == b.x ? (int32)0xffffffff : 0,
            a.y == b.y ? (int32)0xffffffff : 0,
            a.z == b.z ? (int32)0xffffffff : 0,
            a.w == b.w ? (int32)0xffffffff : 0
        };
        #endif
    }

    bool operator!=(SSE_m128Arg a, SSE_m128Arg b) {
        #ifdef DEEP_USE_SSE4_1
        return SSE_m128::Equals(a, b).ToBooleanBitMask() != 0b1111;
        #else
        return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
        #endif
    }
    bool operator==(SSE_m128Arg a, SSE_m128Arg b) {
        return !(a != b);
    }

    SSE_m128& SSE_m128::operator+= (SSE_m128Arg other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_add_ps(_internal, other);
        #else
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        #endif
        return *this;
    }

    SSE_m128 operator+ (SSE_m128 a, SSE_m128Arg b) {
        return a += b;
    }

    SSE_m128& SSE_m128::operator-= (SSE_m128Arg other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_sub_ps(_internal, other);
        #else
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        #endif
        return *this;
    }

    SSE_m128 operator- (SSE_m128 a, SSE_m128Arg b) {
        return a -= b;
    }

    SSE_m128 operator- (SSE_m128Arg a) {
        #ifdef DEEP_USE_SSE4_1
        return _mm_sub_ps(_mm_setzero_ps(), a);
        #else
        // NOTE(randomuserhi): 0.0f - x to stay consistent with vectorised version
        return SSE_m128{
            0.0f - a.x,
            0.0f - a.y,
            0.0f - a.z,
            0.0f - a.w
        };
        #endif
    }

    SSE_m128& SSE_m128::operator*= (SSE_m128Arg other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_mul_ps(_internal, other);
        #else
        x *= other.x;
        y *= other.y;
        z *= other.z;
        w *= other.w;
        #endif
        return *this;
    }
    SSE_m128 operator* (SSE_m128 a, SSE_m128Arg b) {
        return a *= b;
    }

    SSE_m128& SSE_m128::operator*= (float32 other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_mul_ps(_internal, _mm_set1_ps(other));
        #else
        x *= other;
        y *= other;
        z *= other;
        w *= other;
        #endif
        return *this;
    }

    SSE_m128 operator* (SSE_m128 v, float32 a) {
        return v *= a;
    }

    SSE_m128 operator* (float32 a, SSE_m128Arg v) {
        #ifdef DEEP_USE_SSE4_1
        return _mm_mul_ps(_mm_set1_ps(a), v);
        #else
        return SSE_m128{
            a * v.x,
            a * v.y,
            a * v.z,
            a * v.w
        };
        #endif
    }

    SSE_m128& SSE_m128::operator/= (SSE_m128Arg other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_div_ps(_internal, other);
        #else
        x /= other.x;
        y /= other.y;
        z /= other.z;
        w /= other.w;
        #endif
        return *this;
    }
    SSE_m128 operator/ (SSE_m128 a, SSE_m128Arg b) {
        return a /= b;
    }

    SSE_m128& SSE_m128::operator/= (float32 other) {
        #ifdef DEEP_USE_SSE4_1
        _internal = _mm_div_ps(_internal, _mm_set1_ps(other));
        #else
        x /= other;
        y /= other;
        z /= other;
        w /= other;
        #endif
        return *this;
    }

    SSE_m128 operator/ (SSE_m128 v, float32 a) {
        return v /= a;
    }

    SSE_m128 operator/ (float32 a, SSE_m128Arg v) {
        #ifdef DEEP_USE_SSE4_1
        return _mm_div_ps(_mm_set1_ps(a), v);
        #else
        return SSE_m128{
            a / v.x,
            a / v.y,
            a / v.z,
            a / v.w
        };
        #endif
    }

    SSE_m128 SSE_m128::Select(SSE_m128Arg a, SSE_m128Arg b, SSE_m128iArg control) {
        #ifdef DEEP_USE_SSE4_1
        return _mm_blendv_ps(a, b, _mm_castsi128_ps(control));
        #else
        return SSE_m128{
            control.x ? b.x : a.x,
            control.y ? b.y : a.y,
            control.z ? b.z : a.z,
            control.w ? b.w : a.w
        };
        #endif
    }

    void SSE_m128::SinCos(SSE_m128& woSin, SSE_m128& woCos) {
        // Implementation based on sinf.c from the cephes library, combines sinf and cosf in a single function, changes octants to quadrants and vectorizes it
        // Original implementation by Stephen L. Moshier (See: http://www.moshier.net/)

        // Make argument positive and remember sign for sin only since cos is symmetric around x (highest bit of a float is the sign bit)
        SSE_m128i sinSign = SSE_m128i::And(ReinterpretAsInt(), SSE_m128i::Replicate(0x80000000U));
        SSE_m128 x = SSE_m128::Xor(*this, sinSign.ReinterpretAsFloat());

        // x / (PI / 2) rounded to nearest int gives us the quadrant closest to x
        SSE_m128i quadrant = (0.6366197723675814f * x + SSE_m128::Replicate(0.5f)).ToInt();

        // Make x relative to the closest quadrant.
        // This does x = x - quadrant * PI / 2 using a two step Cody-Waite argument reduction.
        // This improves the accuracy of the result by avoiding loss of significant bits in the subtraction.
        // We start with x = x - quadrant * PI / 2, PI / 2 in hexadecimal notation is 0x3fc90fdb, we remove the lowest 16 bits to
        // get 0x3fc90000 (= 1.5703125) this means we can now multiply with a number of up to 2^16 without losing any bits.
        // This leaves us with: x = (x - quadrant * 1.5703125) - quadrant * (PI / 2 - 1.5703125).
        // PI / 2 - 1.5703125 in hexadecimal is 0x39fdaa22, stripping the lowest 12 bits we get 0x39fda000 (= 0.0004837512969970703125)
        // This leaves uw with: x = ((x - quadrant * 1.5703125) - quadrant * 0.0004837512969970703125) - quadrant * (PI / 2 - 1.5703125 - 0.0004837512969970703125)
        // See: https://stackoverflow.com/questions/42455143/sine-cosine-modular-extended-precision-arithmetic
        // After this we have x in the range [-PI / 4, PI / 4].
        SSE_m128 floatQuadrant = quadrant.ToFloat();
        x = ((x - floatQuadrant * 1.5703125f) - floatQuadrant * 0.0004837512969970703125f) - floatQuadrant * 7.549789948768648e-8f;

        // Calculate x2 = x^2
        SSE_m128 x2 = x * x;

        // Taylor expansion:
        // Cos(x) = 1 - x^2/2! + x^4/4! - x^6/6! + x^8/8! + ... = (((x2/8!- 1/6!) * x2 + 1/4!) * x2 - 1/2!) * x2 + 1
        SSE_m128 taylorCos = ((2.443315711809948e-5f * x2 - SSE_m128::Replicate(1.388731625493765e-3f)) * x2 + SSE_m128::Replicate(4.166664568298827e-2f)) * x2 * x2 - 0.5f * x2 + SSE_m128::Replicate(1.0f);
        // Sin(x) = x - x^3/3! + x^5/5! - x^7/7! + ... = ((-x2/7! + 1/5!) * x2 - 1/3!) * x2 * x + x
        SSE_m128 taylorSin = ((-1.9515295891e-4f * x2 + SSE_m128::Replicate(8.3321608736e-3f)) * x2 - SSE_m128::Replicate(1.6666654611e-1f)) * x2 * x + x;

        // The lowest 2 bits of quadrant indicate the quadrant that we are in.
        // Let x be the original input value and x' our value that has been mapped to the range [-PI / 4, PI / 4].
        // since cos(x) = sin(x - PI / 2) and since we want to use the Taylor expansion as close as possible to 0,
        // we can alternate between using the Taylor expansion for sin and cos according to the following table:
        //
        // quadrant	 sin(x)		 cos(x)
        // XXX00b	 sin(x')	 cos(x')
        // XXX01b	 cos(x')	-sin(x')
        // XXX10b	-sin(x')	-cos(x')
        // XXX11b	-cos(x')	 sin(x')
        //
        // So: sin_sign = bit2, cos_sign = bit1 ^ bit2, bit1 determines if we use sin or cos Taylor expansion
        SSE_m128i bit1 = quadrant.LogicalShiftLeft<31>();
        SSE_m128i bit2 = SSE_m128i::And(quadrant.LogicalShiftLeft<30>(), SSE_m128i::Replicate(0x80000000U));

        // Select which one of the results is sin and which one is cos
        SSE_m128 s = SSE_m128::Select(taylorSin, taylorCos, bit1);
        SSE_m128 c = SSE_m128::Select(taylorCos, taylorSin, bit1);

        // Update the signs
        sinSign = SSE_m128i::Xor(sinSign, bit2);
        SSE_m128i cos_sign = SSE_m128i::Xor(bit1, bit2);

        // Correct the signs
        woSin = SSE_m128::Xor(s, sinSign.ReinterpretAsFloat());
        woCos = SSE_m128::Xor(c, cos_sign.ReinterpretAsFloat());
    }
}