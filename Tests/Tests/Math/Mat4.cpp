#include "../pch.h"
#include "Deep.h"
#include "Deep/Math.h"

TEST(Mat4, Equality) {
    const Deep::Vec3 a{ 1, 2, 3 };
    const Deep::Vec3 b{ 1, 2, 3 };

    EXPECT_EQ(a, b);
}