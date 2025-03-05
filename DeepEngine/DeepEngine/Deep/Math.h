/**
 * Math
 *
 * All Math uses float32 and no overloads / template ability is available to use float64 etc...
 * This is because this engine is optimised with float32. Higher precision is not supported.
 */

// NOTE(randomuserhi): Math uses Left-Hand coordinate system so quaternions rotate counter-clockwise ->
//                     https://gamedev.stackexchange.com/questions/87612/quaternion-rotation-clockwise-or-counter-clockwise

#pragma once

#include <Deep.h>
#include <Deep/BitHelper.h>

// Vector alignment for vectorisation
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)

#include <immintrin.h>

#define DEEP_VEC_ALIGNMENT Deep_AlignOf(__m128)

#else

#error Unsupported CPU architecture

#endif

#include <cmath>

namespace Deep {
    // NOTE(randomuserhi):
    // https://stackoverflow.com/questions/13721839/if-stdmax-returns-by-reference-as-it-must-might-that-lead-to-a-dangling-r

    template<typename T>
    Deep_Inline constexpr const T& Min(const T& a, const T& b) {
        return (b < a) ? b : a;
    }

    template<typename T>
    Deep_Inline constexpr const T& Max(const T& a, const T& b) {
        return (b > a) ? b : a;
    }

    template<typename T>
    Deep_Inline constexpr const T& Clamp(const T& x, const T& lower, const T& upper) {
        return Min(upper, Max(x, lower));
    }

    template<typename T>
    Deep_Inline constexpr const T& Clamp01(const T& x) {
        return Clamp(x, 0.0f, 1.0f);
    }

    template<typename T>
    Deep_Inline constexpr const T Sqrt(const T& x) {
        return std::sqrt(x);
    }

    template<typename T>
    Deep_Inline constexpr const T Abs(const T& x) {
        return std::abs(x);
    }

    template<typename T>
    Deep_Inline constexpr T Sign(const T& x) {
        return x < 0 ? static_cast<T>(-1) : static_cast<T>(1);
    }

    template<typename T>
    Deep_Inline constexpr T Sin(const T& x) {
        return std::sin(x);
    }

    template<typename T>
    Deep_Inline constexpr T Cos(const T& x) {
        return std::cos(x);
    }

    template<typename T>
    Deep_Inline constexpr bool isClose(const T& a, const T& b, const T& rtol = 1e-05f, const T& atol = 1e-06f) {
        // NOTE(randomuserhi): https://numpy.org/devdocs/reference/generated/numpy.isclose.html, uses `a` instead `b` as
        // relative point NOTE(randomuserhi): Does not account for NaN
        return Abs(a - b) <= (atol + rtol * Abs(a));
    }
} // namespace Deep

#include <Deep/Math/Constants.h>

// NOTE(randomuserhi): Below import order is important as there are cyclic references that have to be declared first, prior
// implementation.
//                     For this reason, individual math components cannot be imported on their own.

#include "Math/_SSE_m128.h"
#include "Math/_SSE_m128i.h"

#include "Math/SSE_m128.inl"
#include "Math/SSE_m128i.inl"

#include "Math/_Vec3.h"
#include "Math/_Vec4.h"
#include "Math/_Mat4.h"
#include "Math/_Quaternion.h"

#include "Math/Vec3.inl"
#include "Math/Vec4.inl"
#include "Math/Mat4.inl"
#include "Math/Quaternion.inl"
