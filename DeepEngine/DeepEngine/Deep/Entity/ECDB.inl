#pragma once

namespace Deep {
    ECDB::Entt::Entt(ECDB* database, EntityPtr* ptr) :
        ptr(ptr), database(database){};

    ECDB::Entt::operator ECDB::EntityPtr* const() const {
        return ptr;
    }

    ECDB::Entt& ECDB::Entt::AddComponent(ComponentId component) {
        database->AddComponent(ptr, component);
        return *this;
    }
} // namespace Deep

namespace Deep {
    ECDB::ECDB(ECRegistry* registry) :
        registry(registry) {
        rootArchetype = new Archetype(this);
        archetypes.emplace(rootArchetype->description.type, rootArchetype);
    }
} // namespace Deep

namespace Deep {
    ArchetypeBitField::ArchetypeBitField(ECRegistry* registry) :
        registry(registry) {}

    bool ArchetypeBitField::HasComponent(ComponentId component) const {
        uint32 i = component / sizeof(Type);

        if (i < bits.size()) {
            return (bits[i] & (1u << (component % sizeof(Type)))) != 0u;
        }

        return false;
    }
} // namespace Deep

namespace Deep {
    ECDB::ArchetypeDesc::ArchetypeDesc(ECRegistry* registry) :
        registry(registry), type(registry) {}

    bool ECDB::ArchetypeDesc::HasComponent(ComponentId component) const {
        return type.HasComponent(component);
    }

    ECDB::ArchetypeDesc&& ECDB::ArchetypeDesc::AddComponent(ComponentId component) && {
        return std::move(AddComponent(component));
    };

    ECDB::ArchetypeDesc&& ECDB::ArchetypeDesc::RemoveComponent(ComponentId component) && {
        return std::move(RemoveComponent(component));
    };
} // namespace Deep

namespace Deep {
    ECDB::Archetype::Archetype(ECDB* database) :
        database(database), description(database->registry) {}
} // namespace Deep

namespace Deep {
    ECDB::EntityPage::EntityPage(EntityPage* next) :
        next(next) {}
} // namespace Deep