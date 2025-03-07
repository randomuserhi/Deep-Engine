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

            EntityPtr* const ptr;
        };

    private:
        class ArchetypeBitField {
        private:
            using Type = uint32;

        public:
            Deep_Inline ArchetypeBitField(ECRegistry* registry);
            Deep_Inline ArchetypeBitField(ArchetypeBitField& type) = default;
            Deep_Inline ArchetypeBitField(ArchetypeBitField&& type) noexcept;

            inline ArchetypeBitField& operator=(const ArchetypeBitField& ref) = default;
            inline ArchetypeBitField& operator=(ArchetypeBitField&& ref) noexcept = default;

            Deep_Inline bool HasComponent(ComponentId component);

            void AddComponent(ComponentId component);

            void RemoveComponent(ComponentId component);

        private:
            ECRegistry* const registry;

            std::vector<Type> bits;
        };

        class Archetype final : NonCopyable {
            // NOTE(randomuserhi): Handle edge case where archetype is of size 0 (entity with just tags, no components)
            //                     Pretty much just don't allocate any memory at all and the EntityPtr just points to this
            //                     archetype with nullptr chunk.

        public:
            struct Chunk {
                // NOTE(randomuserhi): should appear at the end of the chunk's memory block
                Chunk* next = nullptr;
            };

            Archetype(ECDB* database);
            // Archetype(ECDB* database, ArchetypeBitField type, ComponentDesc* layout, size_t numMembers);
            ~Archetype();

        private:
            void CalcSizeAlignment();

            ECDB* const database;

            ArchetypeBitField type;

            // NOTE(randomuserhi): The order of which components are added determines the memory layout of the components in
            //                     the chunk.
            // NOTE(randomuserhi): Does not include tags.
            std::vector<ComponentDesc> layout;

            // The minimum number of bytes (including padding between members) required to store each component array
            // NOTE(randomuserhi): Not including the extra chunk metadata.
            size_t chunkSize = 0;

            // The alignment of the chunk (based on its component members)
            // NOTE(randomuserhi): Not including the extra chunk metadata.
            size_t chunkAlignment = 0;

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
            //
            // NOTE(randomuserhi): Does not include tags since they dont contribute any data.
            std::vector<ComponentId> ids;
            std::vector<size_t> offsets;

            std::unordered_map<ComponentId, Archetype*> archetypeMap;
        };

        struct EntityPtr {
            Archetype* archetype;
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

            Storage entityLookup[pageSize];
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

        EntityPage::Storage* firstFree = nullptr;

        EntityPage* entityPages = nullptr;
    };

    using Ent = ECDB::Ent;
} // namespace Deep

#include "ECDB.inl"