#include "../pch.h"
#include "Deep.h"
#include "Deep/Math.h"

TEST(Mat4, Equality) {
    const Deep::Mat4 a{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    const Deep::Mat4 b{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    EXPECT_EQ(a, b);
}