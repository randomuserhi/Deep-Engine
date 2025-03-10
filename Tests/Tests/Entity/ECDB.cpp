#include "../pch.h"
#include <Deep.h>
#include <Deep/Entity.h>

TEST(ArchetypeBitField, Hash) {
    Deep::ECRegistry registry;
    Deep::ArchetypeBitField a{ &registry };
    Deep::ArchetypeBitField b{ &registry };

    for (size_t i = 0; i < 64; ++i) {
        registry.RegisterTag();
    }

    a.AddComponent(34);
    EXPECT_NE(std::hash<Deep::ArchetypeBitField>{}(a), std::hash<Deep::ArchetypeBitField>{}(b));

    a.RemoveComponent(34);
    EXPECT_EQ(std::hash<Deep::ArchetypeBitField>{}(a), std::hash<Deep::ArchetypeBitField>{}(b));

    a.AddComponent(1);
    b.AddComponent(1);
    EXPECT_EQ(std::hash<Deep::ArchetypeBitField>{}(a), std::hash<Deep::ArchetypeBitField>{}(b));

    b.RemoveComponent(1);
    EXPECT_NE(std::hash<Deep::ArchetypeBitField>{}(a), std::hash<Deep::ArchetypeBitField>{}(b));
}

struct Component {
    int a;
};

TEST(ECDB, Entity) {
    Deep::ECRegistry registry;
    Deep::ECDB database{ &registry };

    Deep::ComponentId comp = registry.RegisterComponent<Component>();

    Deep::Entt ent = database.Entity();

    ent.AddComponent(comp);
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