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

    const Deep::Mat4 c{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    const Deep::Mat4 d{
        0, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    EXPECT_FALSE(c == d);
}

TEST(Mat4, Multiplication) {
    const Deep::Mat4 a{
        1, 1, 1, 0,
        0, 0, 0, 4,
        0, 3, 2, 0,
        0, 0, 0, 1
    };
    const Deep::Mat4 b{
        2, 3, 0, 1,
        0, 0, 0, 0,
        1, 0, 2, 0,
        0, 0, 4, 0
    };
    const Deep::Mat4 c{
        3, 3, 2, 1,
        0, 0, 16, 0,
        2, 0, 4, 0,
        0, 0, 4, 0
    };

    EXPECT_TRUE((a * b) == c);
}