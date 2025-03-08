#pragma once

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