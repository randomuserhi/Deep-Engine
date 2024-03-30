/**
 * Math
 *
 * All Math uses float32 and no overloads / template ability is available to use float64 etc...
 * This is because this engine is optimised with float32. Higher precision is not supported.
 */

#pragma once

#include "../Deep.h"
#include <cmath>

#define PI 3.141592653589793f

namespace Deep {
    // NOTE(randomuserhi): https://stackoverflow.com/questions/13721839/if-stdmax-returns-by-reference-as-it-must-might-that-lead-to-a-dangling-r

    Deep_Inline const float32& min(const float32& a, const float32& b) {
        return (b < a) ? b : a;
    }

    Deep_Inline const float32& max(const float32& a, const float32& b) {
        return (b > a) ? b : a;
    }

    Deep_Inline const float32& clamp(const float32& x, const float32& upper, const float32& lower) {
        return min(upper, max(x, lower));
    }

    Deep_Inline const float32 sqrt(const float32& x) {
        return std::sqrt(x);
    }
}

#include "./Math/Vec3.h"
#include "./Math/Vec4.h"

#include "./Math/Mat3.h"

#include "./Math/Quaternion.h"