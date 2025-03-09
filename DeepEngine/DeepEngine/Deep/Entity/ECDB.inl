#pragma once

namespace Deep {
    ECDB::Entt::Entt(ECDB* database, EntityPtr* ptr) :
        ptr(ptr), database(database) {};

    ECDB::Entt::operator ECDB::EntityPtr* const() const {
        return ptr;
    }
} // namespace Deep

namespace Deep {
    ECDB::ECDB(ECRegistry* registry) :
        registry(registry), rootArchetype(this) {}
} // namespace Deep

namespace Deep {
    ECDB::ArchetypeBitField::ArchetypeBitField(ECRegistry* registry) :
        registry(registry) {}

    bool ECDB::ArchetypeBitField::HasComponent(ComponentId component) const {
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
} // namespace Deep

namespace Deep {
    ECDB::Archetype::Archetype(ECDB* database) :
        database(database), description(database->registry) {}
} // namespace Deep