#pragma once

namespace Deep {
    bool ArchetypeBitField::HasComponent(ComponentId component) {
        uint32 i = component / sizeof(Type);

        if (i < bits.size()) {
            return (bits[i] & (1u << (component % sizeof(Type)))) != 0u;
        }

        return false;
    }
} // namespace Deep