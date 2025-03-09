#include "../pch.h"
#include <Deep.h>
#include <Deep/Entity.h>

struct Component {
    int a;
};

TEST(ECDB, Entity) {
    Deep::ECRegistry registry;
    Deep::ECDB database{ &registry };

    Deep::Entt ent = database.Entity();
}

TEST(ECDB, Archetype) {
    Deep::ECRegistry registry;
    Deep::ECDB database{ &registry };

    Deep::ComponentId comp[] = { registry.RegisterComponent<Component>(), registry.RegisterComponent<Component>() };

    Deep::ECDB::Archetype& arch = database.GetArchetype(comp, 2);

    const Deep::ECDB::ArchetypeDesc& desc = arch.description;

    // Verify archetype type
    EXPECT_TRUE(desc.HasComponent(comp[0]));
    EXPECT_TRUE(desc.HasComponent(comp[1]));

    // Verify component array offsets (Each should be aligned to DEEP_CACHE_LINE_SIZE)
    EXPECT_EQ(arch.GetComponentOffset(comp[0]), 0);
    EXPECT_EQ(arch.GetComponentOffset(comp[1]), 8192);
}