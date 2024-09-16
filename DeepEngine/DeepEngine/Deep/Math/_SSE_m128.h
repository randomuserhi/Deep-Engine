#pragma once

#include "./_MathTypes.h"
#include <type_traits>

namespace Deep {
    // Abstraction layer for __m128 SIMD type representing 4 independent values
    //
    // Implementation based on Jolt: https://github.com/jrouwe/JoltPhysics/tree/master/Jolt/Math
    struct [[nodiscard]] alignas(DEEP_VEC_ALIGNMENT) SSE_m128 {
        #if defined(DEEP_USE_SSE)
        using Type = __m128;
        #else
        using Type = struct {
            uint32 values[4];
        };
        #endif

        // Constructors
        SSE_m128() = default;
        SSE_m128(const SSE_m128& other) = default;
        SSE_m128& operator= (const SSE_m128& other) = default;
        Deep_Inline SSE_m128(float32 x, float32 y, float32 z, float32 w);
        Deep_Inline SSE_m128(Type _internal);

        // Implicit conversion
        Deep_Inline operator Type() const;

        // Convert into __m128i, converting each component to an integer
        Deep_Inline SSE_m128i ToInt() const;

        // Reinterpret bytes as a __m128i
        Deep_Inline SSE_m128i ReinterpretAsInt() const;

        // Replicate the given value across all components
        static Deep_Inline SSE_m128 Replicate(float32 value);

        // Component wise logical AND
        static Deep_Inline SSE_m128 And(SSE_m128Arg a, SSE_m128Arg b);

        // Component wise logical XOR
        static Deep_Inline SSE_m128 Xor(SSE_m128Arg a, SSE_m128Arg b);

        // Returns a 4 component integer value representing the equality of each component
        // in `a` or `b`.
        // 
        // True is represented by the most significant bit being set.
        static Deep_Inline SSE_m128i Equals(SSE_m128Arg a, SSE_m128Arg b);

        // Component wise select, returns `a` when highest bit of `control` = 0 and `b` when highest bit of `control` = 1
        static Deep_Inline SSE_m128 Select(SSE_m128Arg a, SSE_m128Arg b, SSE_m128iArg control);

        // Equality
        friend Deep_Inline bool operator!=(SSE_m128Arg a, SSE_m128Arg b);
        friend Deep_Inline bool operator==(SSE_m128Arg a, SSE_m128Arg b);

        // Component wise Add
        Deep_Inline SSE_m128& operator+= (SSE_m128Arg other);
        friend Deep_Inline SSE_m128 operator+ (SSE_m128Arg a, SSE_m128Arg b);

        // Component wise Sub
        Deep_Inline SSE_m128& operator-= (SSE_m128Arg other);
        friend Deep_Inline SSE_m128 operator- (SSE_m128Arg a, SSE_m128Arg b);
        friend Deep_Inline SSE_m128 operator- (SSE_m128Arg a);

        // Component wise Mul
        Deep_Inline SSE_m128& operator*= (SSE_m128Arg other);
        friend Deep_Inline SSE_m128 operator* (SSE_m128Arg a, SSE_m128Arg b);

        // Component wise Div
        Deep_Inline SSE_m128& operator/= (SSE_m128Arg other);
        friend Deep_Inline SSE_m128 operator/ (SSE_m128Arg a, SSE_m128Arg b);

        // Mul components with float
        Deep_Inline SSE_m128& operator*= (float32 other);
        friend Deep_Inline SSE_m128 operator* (SSE_m128Arg v, float32 a);
        friend Deep_Inline SSE_m128 operator* (float32 a, SSE_m128Arg v);

        // Div components with float
        Deep_Inline SSE_m128& operator/= (float32 other);
        friend Deep_Inline SSE_m128 operator/ (SSE_m128Arg v, float32 a);
        friend Deep_Inline SSE_m128 operator/ (float32 a, SSE_m128Arg v);

        // Calculate the sin and cosin for each component and store the result
        // in `sin` and `cos` respectively
        inline void SinCos(SSE_m128& woSin, SSE_m128& woCos);

        union {
            Type _internal;
            float32 values[4];
            struct {
                float32 x;
                float32 y;
                float32 z;
                float32 w;
            };
        };
    };

    static_assert(std::is_trivial<SSE_m128>(), "Is supposed to be a trivial type!");
}