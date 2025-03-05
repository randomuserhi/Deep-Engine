#include "../pch.h"
#include <Deep.h>
#include <Deep/Entity/Archetype.h>

// TODO(randomuserhi): Split this test up
TEST(Archetype, ArchetypeBitField) {
    Deep::ArchetypeBitField bitfield;

    EXPECT_FALSE(bitfield.HasComponent(32u));

    bitfield.AddComponent(32u);
    EXPECT_TRUE(bitfield.HasComponent(32u));

    bitfield.AddComponent(2u);
    EXPECT_TRUE(bitfield.HasComponent(2u));

    bitfield.RemoveComponent(32u);
    EXPECT_FALSE(bitfield.HasComponent(32u));

    EXPECT_TRUE(bitfield.HasComponent(2u));
}