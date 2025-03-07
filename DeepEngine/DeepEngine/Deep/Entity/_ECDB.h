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

            // NOTE(randomuserhi): The order of which components are added determines the order in memory within a chunk
            std::vector<ComponentDesc> components;
        };

        class Archetype final : NonCopyable {
        public:
            struct Chunk {
                // NOTE(randomuserhi): should appear at the start of a chunk's memory block (chunk header)
                Archetype* archetype;

                // NOTE(randomuserhi): should appear at the end of the chunk's memory block (chunk footer)
                Chunk* next = nullptr;
            };

            Archetype(ECDB* database, ArchetypeType type);
            ~Archetype();

        private:
            ECDB* const database;

            const ArchetypeType type;

            // The minimum number of bytes (including padding between members) required to store each component array
            // NOTE(randomuserhi): Not including the chunk header / chunk footer.
            const size_t chunkSize;

            // The alignment of the chunk (based on its component members)
            // NOTE(randomuserhi): Not including the chunk header / chunk footer.
            const size_t chunkAlignment;

            Chunk* firstFree = nullptr;

            Chunk* chunks = nullptr;

            // Instead of the above, finding the offset of a component within a chunk is done using the 2 buckets
            // `ids` and `offsets`. Each item in `ids` and `offsets` correspond to each other so `offsets[0]` is the offset
            // for ComponentId `ids[0]`.
            //
            // When looking up the offset for a given ComponentId, (ComponentId % numComponentsInArchetype) is used to index
            // the `ids` bucket. A linear probe is then used to resolve hash conflicts (iterating `ids` until a match is
            // found). The index of the match is then used to lookup the offset from the `offsets` bucket.
            //
            // This is a more cache efficient `std::unordered_map<ComponentId, size_t>` specialized for the constraints of:
            // - Fixed number of components in an archetype
            // - ComponentIds are numbers
            std::vector<ComponentId> ids;
            std::vector<size_t> offsets;

            std::unordered_map<ComponentId, Archetype*> archetypeMap;
        };

        struct EntityPtr {
            Archetype::Chunk* chunk;
            size_t index;
        };

        struct EntityPage {
            struct Storage {
                EntityPtr ptr;
                Storage* next;
            };

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