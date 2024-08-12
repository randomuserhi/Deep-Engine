#include "../pch.h"
#include "Deep.h"
#include "Deep/Math.h"

TEST(Vec4, Equality) {
    Deep::Vec4 a{ 1, 2, 3, 4 };
    Deep::Vec4 b{ 1, 2, 3, 4 };

    EXPECT_EQ(a, b);
}

TEST(Vec4, Add) {
    Deep::Vec4 a{ 1, 2, 3, 4 };
    Deep::Vec4 b{ 4, 3, 2, 1 };
    Deep::Vec4 c = a + b;
    EXPECT_EQ(c.x, 5);
    EXPECT_EQ(c.y, 5);
    EXPECT_EQ(c.z, 5);
    EXPECT_EQ(c.w, 5);

    a += b;
    EXPECT_EQ(a.x, 5);
    EXPECT_EQ(a.y, 5);
    EXPECT_EQ(a.z, 5);
    EXPECT_EQ(a.w, 5);
}

TEST(Vec4, Sub) {
    Deep::Vec4 a{ 1, 2, 3, 4 };
    Deep::Vec4 b{ 4, 3, 2, 1 };
    Deep::Vec4 c = a - b;
    EXPECT_EQ(c.x, -3);
    EXPECT_EQ(c.y, -1);
    EXPECT_EQ(c.z, 1);
    EXPECT_EQ(c.w, 3);

    a -= b;
    EXPECT_EQ(a.x, -3);
    EXPECT_EQ(a.y, -1);
    EXPECT_EQ(a.z, 1);
    EXPECT_EQ(a.w, 3);
}

TEST(Vec4, Scale) {
    {
        Deep::Vec4 a{ 1, 2, 3, 4 };
        Deep::Vec4 b = a * 2;
        EXPECT_EQ(b.x, 2);
        EXPECT_EQ(b.y, 4);
        EXPECT_EQ(b.z, 6);
        EXPECT_EQ(b.w, 8);

        a *= 2;
        EXPECT_EQ(a.x, 2);
        EXPECT_EQ(a.y, 4);
        EXPECT_EQ(a.z, 6);
        EXPECT_EQ(a.w, 8);
    }

    {
        Deep::Vec4 a{ 1, 2, 3, 4 };
        Deep::Vec4 b = a / 2;
        EXPECT_EQ(b.x, 0.5f);
        EXPECT_EQ(b.y, 1.0f);
        EXPECT_EQ(b.z, 1.5f);
        EXPECT_EQ(b.w, 2.0f);

        a /= 2;
        EXPECT_EQ(a.x, 0.5f);
        EXPECT_EQ(a.y, 1.0f);
        EXPECT_EQ(a.z, 1.5f);
        EXPECT_EQ(a.w, 2.0f);
    }
}

TEST(Vec4, Dot) {
    Deep::Vec4 a{ 1, 2, 3, 4 };
    Deep::Vec4 b{ 4, 3, 2, 1 };
    float32 c = Deep::Vec4::Dot(a, b);
    float32 d = Deep::Vec4::Dot(b, a);
    EXPECT_EQ(c, 20);
    EXPECT_EQ(d, 20);
}

TEST(Vec4, Mul) {
    Deep::Vec4 a{ 1, 2, 3, 4 };
    Deep::Vec4 b{ 4, 3, 2, 1 };
    Deep::Vec4 c = a * b;
    EXPECT_EQ(c.x, 4.0f);
    EXPECT_EQ(c.y, 6.0f);
    EXPECT_EQ(c.z, 6.0f);
    EXPECT_EQ(c.w, 4.0f);

    a *= b;
    EXPECT_EQ(a.x, 4.0f);
    EXPECT_EQ(a.y, 6.0f);
    EXPECT_EQ(a.z, 6.0f);
    EXPECT_EQ(a.w, 4.0f);
}

TEST(Vec4, Div) {
    Deep::Vec4 a{ 1, 2, 3, 4 };
    Deep::Vec4 b{ 4, 3, 2, 1 };
    Deep::Vec4 c = a / b;
    EXPECT_EQ(c.x, 1.0f / 4.0f);
    EXPECT_EQ(c.y, 2.0f / 3.0f);
    EXPECT_EQ(c.z, 3.0f / 2.0f);
    EXPECT_EQ(c.w, 4.0f);

    a /= b;
    EXPECT_EQ(a.x, 1.0f / 4.0f);
    EXPECT_EQ(a.y, 2.0f / 3.0f);
    EXPECT_EQ(a.z, 3.0f / 2.0f);
    EXPECT_EQ(a.w, 4.0f);
}