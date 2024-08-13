#pragma once

#include "./_MathTypes.h"
#include <type_traits>

namespace Deep {
    // Abstraction layer for __m128i SIMD type representing 4 independent values
    struct [[nodiscard]] alignas(DEEP_VEC_ALIGNMENT) SSE_m128i {
        #if defined(DEEP_USE_SSE)
        using Type = __m128i;
        #else
        using Type = struct {
            uint32 values[4];
        };
        #endif

        // Constructors
        SSE_m128i() = default;
        SSE_m128i(const SSE_m128i& other) = default;
        SSE_m128i& operator= (const SSE_m128i& other) = default;
        Deep_Inline SSE_m128i(int32 x, int32 y, int32 z, int32 w);
        Deep_Inline SSE_m128i(Type _internal);

        // Implicit conversion
        Deep_Inline operator Type() const;

        // Convert into __m128, converting each component to a float
        Deep_Inline SSE_m128 ToFloat() const;

        // Reinterpret bytes as a __m128
        Deep_Inline SSE_m128 ReinterpretAsFloat() const;

        // Treats the 4 components as booleans where true is when the most significant bit is set
        // and returns it as a bit mask.
        //
        // Bit 0 is set if X is true, Bit 1 is set if Y is true, Bit 2 is set if Z is true and Bit 3 is set if W is true.
        Deep_Inline int32 ToBooleanBitMask() const;

        // Replicate the given value across all components
        static Deep_Inline SSE_m128i Replicate(int value);

        // Component wise logical AND
        static Deep_Inline SSE_m128i And(SSE_m128iArg a, SSE_m128iArg b);

        // Component wise logical XOR
        static Deep_Inline SSE_m128i Xor(SSE_m128iArg a, SSE_m128iArg b);

        // Returns a 4 component integer value representing the equality of each component
        // in `a` or `b`.
        // 
        // True is represented by the most significant bit being set.
        static Deep_Inline SSE_m128i Equals(SSE_m128iArg a, SSE_m128iArg b);

        // Shift all components by `Count` bits to the left
        template <const uint Count>
        Deep_Inline SSE_m128i LogicalShiftLeft() const;

        // Shift all components by `Count` bits to the right
        template <const uint Count>
        Deep_Inline SSE_m128i LogicalShiftRight() const;

        // Component wise select, returns `a` when highest bit of `control` = 0 and `b` when highest bit of `control` = 1
        static Deep_Inline SSE_m128i Select(SSE_m128iArg a, SSE_m128iArg b, SSE_m128iArg control);

        // Equality
        friend Deep_Inline bool operator!=(SSE_m128iArg a, SSE_m128iArg b);
        friend Deep_Inline bool operator==(SSE_m128iArg a, SSE_m128iArg b);

        // Component wise Add
        Deep_Inline SSE_m128i& operator+= (SSE_m128iArg other);
        friend Deep_Inline SSE_m128i operator+ (SSE_m128iArg a, SSE_m128iArg b);

        // Component wise Sub
        Deep_Inline SSE_m128i& operator-= (SSE_m128iArg other);
        friend Deep_Inline SSE_m128i operator- (SSE_m128iArg a, SSE_m128iArg b);
        friend Deep_Inline SSE_m128i operator- (SSE_m128iArg a);

        // Component wise Mul
        Deep_Inline SSE_m128i& operator*= (SSE_m128iArg other);
        friend Deep_Inline SSE_m128i operator* (SSE_m128iArg a, SSE_m128iArg b);

        // Component wise Div
        Deep_Inline SSE_m128i& operator/= (SSE_m128iArg other);
        friend Deep_Inline SSE_m128i operator/ (SSE_m128iArg a, SSE_m128iArg b);

        // Mul components with float
        Deep_Inline SSE_m128i& operator*= (int32 other);
        friend Deep_Inline SSE_m128i operator* (SSE_m128iArg v, int32 a);
        friend Deep_Inline SSE_m128i operator* (int32 a, SSE_m128iArg v);

        // Div components with float
        Deep_Inline SSE_m128i& operator/= (int32 other);
        friend Deep_Inline SSE_m128i operator/ (SSE_m128iArg v, int32 a);
        friend Deep_Inline SSE_m128i operator/ (int32 a, SSE_m128iArg v);

        union {
            Type _internal;
            int32 values[4];
            struct {
                int32 x;
                int32 y;
                int32 z;
                int32 w;
            };
        };
    };

    static_assert(std::is_trivial<SSE_m128i>(), "Is supposed to be a trivial type!");
}