#include "../pch.h"
#include <Deep.h>
#include <Deep/Entity/ECRegistry.h>
#include <string>

struct Component {
    int32 a;
};

struct Tag {};

TEST(ECStaticRegistry, Components) {
    Deep::ECRegistry _registry;
    Deep::ECStaticRegistry registry{ &_registry };

    Deep::ComponentId id = registry.RegisterComponent<Component>();

    EXPECT_EQ(id, registry.Get<Component>());
    EXPECT_EQ(_registry[id].size, sizeof(Component));
    EXPECT_EQ(_registry[id].alignment, alignof(Component));

    // NOTE(randomuserhi): `_registry[id].name` is implementation defined, thus we cannot test it reliably
}

TEST(ECStaticRegistry, Tags) {
    Deep::ECRegistry _registry;
    Deep::ECStaticRegistry registry{ &_registry };

    Deep::ComponentId id = registry.RegisterTag<Tag>();

    EXPECT_EQ(id, registry.Get<Tag>());
    EXPECT_EQ(_registry[id].size, 0);
    EXPECT_EQ(_registry[id].alignment, 0);

    // NOTE(randomuserhi): `_registry[id].name` is implementation defined, thus we cannot test it reliably
}

TEST(ECRegistry, Components) {
    Deep::ECRegistry registry;

    const char* name = "Component";
    Deep::ComponentId id = registry.RegisterComponent<Component>(name);

    EXPECT_EQ(registry[id].size, sizeof(Component));
    EXPECT_EQ(registry[id].alignment, alignof(Component));
    EXPECT_EQ(registry[id].name, name);
}

TEST(ECRegistry, Tags) {
    Deep::ECRegistry registry;

    const char* name = "Tag";
    Deep::ComponentId id = registry.RegisterTag<Tag>(name);

    EXPECT_EQ(registry[id].size, 0);
    EXPECT_EQ(registry[id].alignment, 0);
    EXPECT_EQ(registry[id].name, name);
}