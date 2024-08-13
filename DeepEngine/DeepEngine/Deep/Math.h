/**
 * Math
 *
 * All Math uses float32 and no overloads / template ability is available to use float64 etc...
 * This is because this engine is optimised with float32. Higher precision is not supported.
 */

// NOTE(randomuserhi): Math uses Left-Hand coordinate system so quaternions rotate counter-clockwise -> 
//                     https://gamedev.stackexchange.com/questions/87612/quaternion-rotation-clockwise-or-counter-clockwise

#pragma once

#include "../Deep.h"
#include "./BitHelper.h"

// Vector alignment for vectorisation
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)

#include <immintrin.h>

#define DEEP_VEC_ALIGNMENT Deep_AlignOf(__m128)

#else

#error Unsupported CPU architecture

#endif

#include <cmath>
#include <cassert>

#define PI 3.141592653589793f

namespace Deep {
    // NOTE(randomuserhi): https://stackoverflow.com/questions/13721839/if-stdmax-returns-by-reference-as-it-must-might-that-lead-to-a-dangling-r

    Deep_Inline const float32& Min(const float32& a, const float32& b) {
        return (b < a) ? b : a;
    }

    Deep_Inline const float32& Max(const float32& a, const float32& b) {
        return (b > a) ? b : a;
    }

    Deep_Inline const float32& Clamp(const float32& x, const float32& lower, const float32& upper) {
        return Min(upper, Max(x, lower));
    }

    Deep_Inline const float32& Clamp01(const float32& x) {
        return Clamp(x, 0.0f, 1.0f);
    }

    Deep_Inline const float32 Sqrt(const float32& x) {
        return std::sqrt(x);
    }

    Deep_Inline const float32 Abs(const float32& x) {
        return std::abs(x);
    }
}

// NOTE(randomuserhi): Below import order is important as there are cyclic references that have to be declared first, prior implementation.
//                     For this reason, individual math components cannot be imported on their own.

#include "./Math/_SSE_m128.h"
#include "./Math/_SSE_m128i.h"

#include "./Math/SSE_m128.inl"
#include "./Math/SSE_m128i.inl"

#include "./Math/_Vec3.h"
#include "./Math/_Vec4.h"
#include "./Math/_Mat4.h"
#include "./Math/_Quaternion.h"

#include "./Math/Vec3.inl"
#include "./Math/Vec4.inl"
#include "./Math/Mat4.inl"
#include "./Math/Quaternion.inl"
