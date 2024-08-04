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

    a += b;
    EXPECT_EQ(a.x, 4);
    EXPECT_EQ(a.y, 4);
    EXPECT_EQ(a.z, 4);
}

TEST(Vec3, Sub) {
    Deep::Vec3 a{ 1, 2, 3 };
    Deep::Vec3 b{ 3, 2, 1 };
    Deep::Vec3 c = a - b;
    EXPECT_EQ(c.x, -2);
    EXPECT_EQ(c.y, 0);
    EXPECT_EQ(c.z, 2);

    a -= b;
    EXPECT_EQ(a.x, -2);
    EXPECT_EQ(a.y, 0);
    EXPECT_EQ(a.z, 2);
}

TEST(Vec3, Scale) {
    {
        Deep::Vec3 a{ 1, 2, 3 };
        Deep::Vec3 b = a * 2;
        EXPECT_EQ(b.x, 2);
        EXPECT_EQ(b.y, 4);
        EXPECT_EQ(b.z, 6);

        a *= 2;
        EXPECT_EQ(a.x, 2);
        EXPECT_EQ(a.y, 4);
        EXPECT_EQ(a.z, 6);
    }

    {
        Deep::Vec3 a{ 1, 2, 3 };
        Deep::Vec3 b = a / 2;
        EXPECT_EQ(b.x, 0.5f);
        EXPECT_EQ(b.y, 1.0f);
        EXPECT_EQ(b.z, 1.5f);

        a /= 2;
        EXPECT_EQ(a.x, 0.5f);
        EXPECT_EQ(a.y, 1.0f);
        EXPECT_EQ(a.z, 1.5f);
    }
}

TEST(Vec3, Dot) {
    Deep::Vec3 a{ 1, 2, 3 };
    Deep::Vec3 b{ 3, 2, 1 };
    float32 c = a * b;
    float32 d = b * a;
    EXPECT_EQ(c, 10);
    EXPECT_EQ(d, 10);
}