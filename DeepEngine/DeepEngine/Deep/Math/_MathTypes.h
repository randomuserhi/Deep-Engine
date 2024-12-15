#pragma once

#include "../../Deep.h"

// Implementation based on Jolt: https://github.com/jrouwe/JoltPhysics/tree/master/Jolt/Math

namespace Deep {
    struct SSE_m128;
    struct SSE_m128i;

    struct Vec3;
    struct Vec4;
    struct Quat;

    struct Mat4;

    // Types to use for passing arguments to functions.
    // These types represent the fastest way to pass a given type as a read-only argument. (E.g passing Mat4 by reference
    // will be faster than by value)
    //
    // If you need specific access patterns do not use these types.

    using SSE_m128Arg = const SSE_m128;
    using SSE_m128iArg = const SSE_m128i;

    using Vec3Arg = const Vec3;
    using Vec4Arg = const Vec4;
    using QuatArg = const Quat;
    using Mat4Arg = const Mat4&;
} // namespace Deep