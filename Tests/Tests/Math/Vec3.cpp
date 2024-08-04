#include "../pch.h"
#include "Deep.h"
#include "Deep/Math.h"

TEST(Vec3, Equality) {
    Deep::Vec3 a{ 1, 2, 3 };
    Deep::Vec3 b{ 1, 2, 3 };
    EXPECT_EQ(a, b);
}

TEST(Vec3, Add) {
    Deep::Vec3 a{ 1, 2, 3 };
    Deep::Vec3 b{ 3, 2, 1 };
    Deep::Vec3 c = a + b;
    EXPECT_EQ(c.x, 4);
    EXPECT_EQ(c.y, 4);
    EXPECT_EQ(c.z, 4);
}