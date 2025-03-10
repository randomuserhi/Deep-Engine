#include "../pch.h"
#include <Deep.h>
#include <Deep/Entity.h>

TEST(ArchetypeBitField, Hash) {
    Deep::ECRegistry registry;
    Deep::ArchetypeBitField a{ &registry };
    Deep::ArchetypeBitField b{ &registry };

    for (size_t i = 0; i < 64; ++i) {
        registry.RegisterComponent(1, 1);
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

// TODO(randomuserhi): Make this neater -> split by scoping
TEST(ECDB, Entity) {
    Deep::ECRegistry registry;
    Deep::ECDB database{ &registry };

    Deep::Entt entt = database.Entity();

    Deep::ECDB::Archetype& root = database.GetRootArchetype();

    // Check metadata matches
    const Deep::ECDB::Archetype::Metadata& rootMeta = root.GetMetadata(entt);
    EXPECT_EQ(rootMeta.entt, entt);

    // Check entity archetype changes
    Deep::ComponentId comp = registry.RegisterComponent<Component>();
    entt.AddComponent(comp);

    Deep::ComponentId compArr[] = { comp };
    Deep::ECDB::Archetype& arch = database.GetArchetype(compArr, 1);

    // Check metadata matches
    const Deep::ECDB::Archetype::Metadata& metadata = arch.GetMetadata(entt);
    EXPECT_EQ(metadata.entt, entt);

    {
        Component& c = arch.GetComponent<Component>(entt, comp);
        c.a = 10;
    }

    Deep::ComponentId tag = registry.RegisterTag();
    entt.AddComponent(tag);

    {
        Deep::ComponentId tagArr[] = { comp, tag };
        Deep::ECDB::Archetype& tagArch = database.GetArchetype(tagArr, 2);

        Component& c = tagArch.GetComponent<Component>(entt, comp);
        EXPECT_EQ(c.a, 10);
    }

    entt.RemoveComponent(comp);
    entt.RemoveComponent(tag);

    // Check metadata matches
    EXPECT_EQ(rootMeta.entt, entt);
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
    EXPECT_EQ(arch.GetComponentOffset(comp[0]).offset, 8192);
    EXPECT_EQ(arch.GetComponentOffset(comp[1]).offset, 12288);
}