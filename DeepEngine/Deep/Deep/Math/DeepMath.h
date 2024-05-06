#pragma once

#include <cmath>

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
}