#include "./pch.h"
#include <Deep.h>
#include <Deep/BitHelper.h>

TEST(BitHelper, NumberTrailingZeros) {
    EXPECT_EQ(Deep::NumTrailingZeros(0u), 32u);
    EXPECT_EQ(Deep::NumTrailingZeros(1u), 0u);
    EXPECT_EQ(Deep::NumTrailingZeros(2u), 1u);
    EXPECT_EQ(Deep::NumTrailingZeros(4u), 2u);
    EXPECT_EQ(Deep::NumTrailingZeros(0xFFFFFFFFu), 0u);
}