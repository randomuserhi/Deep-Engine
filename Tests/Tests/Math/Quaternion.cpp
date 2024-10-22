#include "../pch.h"
#include "Deep.h"
#include "Deep/Math.h"

TEST(Quaternion, Equality) {
    const Deep::Quat a{ 0.0f, 0.0f, 0.0f, 1.0f };
    const Deep::Quat b{ 0.0f, 0.0f, 0.0f, 1.0f };

    EXPECT_TRUE(a == b);
}

TEST(Quaternion, Conjugate) {
    const Deep::Quat a{ 0.0f, 1.0f, 0.0f, 0.0f };
    const Deep::Quat b = a.conjugated();

    EXPECT_EQ(b.x, -0.0f);
    EXPECT_EQ(b.y, -1.0f);
    EXPECT_EQ(b.z, -0.0f);
    EXPECT_EQ(b.w, 0.0f);
}

TEST(Quaternion, FromAxisAngle) {
    const Deep::Quat a{ { 0.0f, 1.0f, 0.0f }, Deep::Pi };

    EXPECT_TRUE(Deep::isClose(a.x, 0.0f));
    EXPECT_TRUE(Deep::isClose(a.y, 1.0f));
    EXPECT_TRUE(Deep::isClose(a.z, 0.0f));
    EXPECT_TRUE(Deep::isClose(a.w, 0.0f));
}

TEST(Quaternion, RotateVector) {
    const Deep::Quat q{ { 0.0f, 1.0f, 0.0f }, 0.5 * Deep::Pi };
    const Deep::Vec3 v{ 1.0f, 0.0f, 0.0f };
    const Deep::Vec3 result = q * v;

    EXPECT_TRUE(Deep::isClose(result.x, 0.0f));
    EXPECT_TRUE(Deep::isClose(result.y, 0.0f));
    EXPECT_TRUE(Deep::isClose(result.z, -1.0f));
}

TEST(Quaternion, InverseRotateVector) {
    Deep::Quat q{ { 0.0f, 1.0f, 0.0f }, 0.5 * Deep::Pi };
    const Deep::Vec3 v{ 1.0f, 0.0f, 0.0f };
    const Deep::Vec3 result = Deep::Quat::InverseRotate(q, v);

    EXPECT_TRUE(Deep::isClose(result.x, 0.0f));
    EXPECT_TRUE(Deep::isClose(result.y, 0.0f));
    EXPECT_TRUE(Deep::isClose(result.z, 1.0f));
}