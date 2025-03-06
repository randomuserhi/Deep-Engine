#pragma once

// TODO(randomuserhi): Remove this include, its unnecessary but VS is being stupid
#include <Deep/Entity.h>

#include <Deep/NonCopyable.h>

#include <unordered_map>
#include <vector>

namespace Deep {

    // TODO(randomuserhi): Asserts and debug mode memory safety (counting number of delete calls etc...)

    class ECDB final : NonCopyable {
    private:
        struct EntityPtr;

    public:
        struct Ent {
            Ent() = delete;
            Deep_Inline Ent(ECDB* database, EntityPtr* ptr) :
                ptr(ptr), database(database) {}; // TODO(randomuserhi): move somewhere else

        private:
            ECDB* const database;

            EntityPtr* ptr;
        };

    private:
        class ArchetypeType {
        private:
            using Type = uint32;

        public:
            Deep_Inline ArchetypeType(ECRegistry* registry);
            Deep_Inline ArchetypeType(ArchetypeType& type) = default;
            Deep_Inline ArchetypeType(ArchetypeType&& type) noexcept;

            inline ArchetypeType& operator=(const ArchetypeType& ref) = default;
            inline ArchetypeType& operator=(ArchetypeType&& ref) noexcept = default;

            Deep_Inline bool HasComponent(ComponentId component);

            void AddComponent(ComponentId component);

            void RemoveComponent(ComponentId component);

            size_t Size();

            size_t Alignment();

        private:
            ECRegistry* const registry;

            std::vector<Type> bits;
            std::vector<ComponentDesc> components;
        };

        class Archetype final : NonCopyable {
        public:
            struct Chunk {
                Chunk* next = nullptr;
            };

            Archetype(ECDB* database, ArchetypeType type);
            ~Archetype();

        private:
            ECDB* const database;

            const ArchetypeType type;

            const size_t chunkSize;
            const size_t chunkAlignment;

            Chunk* firstFree = nullptr;

            Chunk* chunks = nullptr;

            std::unordered_map<ComponentId, Archetype*> archetypeMap;
        };

        struct EntityPtr {
            Archetype::Chunk* chunk;

            size_t index;

            EntityPtr* next;
        };

        struct EntityPage {
            static const size_t pageSize = 85;

            EntityPage* next;

            EntityPtr entityLookup[pageSize];
        };

    public:
        ECDB(ECRegistry* registry);
        ~ECDB();

        ECDB::Ent Entity();

    private:
        ECRegistry* const registry;

        std::vector<Archetype*> archetypes;

        std::unordered_map<ComponentId, Archetype*> archetypeMap;

        uint32 firstFreeItemInNewPage = 0;

        EntityPtr* firstFree = nullptr;

        EntityPage* entityPages = nullptr;
    };

    using Ent = ECDB::Ent;
} // namespace Deep

#include "ECDB.inl"