#include <iostream>

#include <Deep.h>
#include <Deep/Entity.h>

struct Component {
    int a;
};

void Loop1(std::vector<Component>& comps) {
    for (size_t i = 0; i < comps.size(); ++i) {
        Component& t = comps[i];
        t.a = i;
    }
}

void Loop2(Deep::ECDB::Archetype& arch, Deep::ComponentId comp) {
    Deep::ECDB::Archetype::ComponentOffset offset = arch.GetComponentOffset(comp);
    for (Deep::ECDB::Archetype::Chunk* c = arch.chunks(); c != nullptr; c = c->next) {
        Component* comps = arch.GetCompList<Component>(c, offset);
        for (size_t i = 0; i < arch.GetChunkSize(c); ++i) {
            Component& t = comps[i];
            t.a = i;
        }
    }
}

int main() {
    Deep::ECRegistry registry;
    Deep::ECDB database{ &registry };

    Deep::ComponentId comp = registry.RegisterComponent<Component>();
    Deep::ComponentId compArr[] = { comp };
    Deep::ECDB::Archetype& arch = database.GetArchetype(compArr, 1);

    size_t numEntities = 10000;

    for (size_t i = 0; i < numEntities; ++i) {
        arch.Entity();
    }

    std::vector<Component> comps{ 10000 };

    Loop1(comps);

    for (size_t i = 0; i < comps.size(); ++i) {
        std::cout << comps[i].a << '\n';
    }

    Loop2(arch, comp);
}