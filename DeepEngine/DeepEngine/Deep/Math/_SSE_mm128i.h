#pragma once

#include "./_MathTypes.h"
#include <type_traits>

namespace Deep {
    struct [[nodiscard]] alignas(DEEP_VEC_ALIGNMENT) SSE_mm128i {
        #if defined(DEEP_USE_SSE)
        using Type = __m128i;
        #else
        using Type = struct {
            uint32 values[4];
        };
        #endif

        SSE_mm128i() = default;
        SSE_mm128i(const SSE_mm128i& other) = default;
        SSE_mm128i& operator= (const SSE_mm128i& other) = default;
        Deep_Inline SSE_mm128i(int32 x, int32 y, int32 z, int32 w);
        //Deep_Inline SSE_mm128i(uint32 x, uint32 y, uint32 z, uint32 w); // NOTE(randomuserhi): Provided for simplicity when passing raw values -> all values still get truncated to int32
        Deep_Inline SSE_mm128i(Type _internal);

        Deep_Inline operator Type() const;

        Deep_Inline SSE_mm128 ToFloat() const;
        Deep_Inline SSE_mm128 ReinterpretAsFloat() const;

        static Deep_Inline SSE_mm128i Replicate(int value);
        static Deep_Inline SSE_mm128i Xor(SSE_mm128i a, SSE_mm128i b);
        static Deep_Inline SSE_mm128i And(SSE_mm128i a, SSE_mm128i b);

        template <const uint Count>
        Deep_Inline SSE_mm128i LogicalShiftLeft() const;
        template <const uint Count>
        Deep_Inline SSE_mm128i LogicalShiftRight() const;

        static Deep_Inline SSE_mm128i Select(SSE_mm128i a, SSE_mm128i b, SSE_mm128i control);

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

    static_assert(std::is_trivial<SSE_mm128i>(), "Is supposed to be a trivial type!");
}