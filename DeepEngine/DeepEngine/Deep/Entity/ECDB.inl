#pragma once

namespace Deep {
    ECDB::Entt::Entt(ECDB* database, EntityPtr* ptr) :
        ptr(ptr), database(database) {};
} // namespace Deep

namespace Deep {
    ECDB::ECDB(ECRegistry* registry) :
        registry(registry) {}
} // namespace Deep

namespace Deep {
    ECDB::ArchetypeBitField::ArchetypeBitField(ECRegistry* registry) :
        registry(registry) {}

    ECDB::ArchetypeBitField::ArchetypeBitField(ArchetypeBitField&& type) noexcept :
        registry(type.registry), bits(std::move(type.bits)) {}

    bool ECDB::ArchetypeBitField::HasComponent(ComponentId component) {
        uint32 i = component / sizeof(Type);

        if (i < bits.size()) {
            return (bits[i] & (1u << (component % sizeof(Type)))) != 0u;
        }

        return false;
    }
} // namespace Deep

namespace Deep {
    ECDB::ArchetypeDesc::ArchetypeDesc(ECDB* database) :
        database(database), type(database->registry) {}

    ECDB::ArchetypeDesc::ArchetypeDesc(ArchetypeDesc&& description) noexcept :
        database(database), type(std::move(description.type)), layout(std::move(description.layout)) {}
} // namespace Deep

namespace Deep {
    ECDB::Archetype::Archetype(ECDB* database) :
        description(database) {}
} // namespace Deep