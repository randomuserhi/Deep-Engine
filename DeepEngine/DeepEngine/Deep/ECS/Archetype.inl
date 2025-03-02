#pragma once

namespace Deep {
    bool ArchetypeBitField::HasComponent(ComponentId component) {
        uint32 i = component / sizeof(Type);

        if (i < bits.size()) {
            return (bits[i] & (1u << (component % sizeof(Type)))) != 0u;
        }

        return false;
    }

    void ArchetypeBitField::AddComponent(ComponentId component) {
        uint32 i = component / sizeof(Type);
        while (i >= bits.size()) {
            bits.emplace_back(0u);
        }
        bits[i] |= (1u << (component % sizeof(Type)));
    }

    void ArchetypeBitField::RemoveComponent(ComponentId component) {
        uint32 i = component / sizeof(Type);
        if (i < bits.size()) {
            bits[i] &= ~(1u << (component % sizeof(Type)));
        }
    }

} // namespace Deep