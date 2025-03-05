#pragma once

#include <Deep.h>
#include <Deep/Entity.h>
#include <Deep/NonCopyable.h>

#include <vector>

// TODO(randomuserhi): Documentation

namespace Deep {
    struct ArchetypeBitField final : private NonCopyable {
    private:
        using Type = uint32;

    public:
        Deep_Inline bool HasComponent(ComponentId component);

        void AddComponent(ComponentId component);

        void RemoveComponent(ComponentId component);

    private:
        std::vector<Type> bits;
    };
} // namespace Deep

#include "Archetype.inl"