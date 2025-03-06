#pragma once

namespace Deep {
    ECDB::ArchetypeType::ArchetypeType(ECRegistry* registry) :
        registry(registry) {}

    ECDB::ArchetypeType::ArchetypeType(ArchetypeType&& type) noexcept :
        registry(type.registry), bits(std::move(type.bits)), components(std::move(type.components)) {}

    bool ECDB::ArchetypeType::HasComponent(ComponentId component) {
        uint32 i = component / sizeof(Type);

        if (i < bits.size()) {
            return (bits[i] & (1u << (component % sizeof(Type)))) != 0u;
        }

        return false;
    }
} // namespace Deep