#pragma once

#include "../../Deep.h"

#include "../NonCopyable.h"

#include <vector>

// TODO(randomuserhi): Documentation

namespace Deep {
    using ComponentId = uint32; // TODO(randomuserhi): Move somewhere else

    struct ArchetypeBitField final : private NonCopyable {
    private:
        using Type = uint32;

    public:
        Deep_Inline bool HasComponent(ComponentId component);

        Deep_Inline void AddComponent(ComponentId component);

        Deep_Inline void RemoveComponent(ComponentId component);

    private:
        std::vector<Type> bits;
    };
} // namespace Deep

#include "./Archetype.inl"