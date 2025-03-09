#pragma once

// TODO(randomuserhi): Remove this include, its unnecessary but VS is being stupid
#include <Deep/Bithelper.h>
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
        struct Entt {
            explicit Deep_Inline Entt(ECDB* database, EntityPtr* ptr);

            Deep_Inline operator ECDB::EntityPtr* const() const;

        private:
            ECDB* const database;

            EntityPtr* const ptr;
        };

    private:
        struct ArchetypeBitField {
        private:
            using Type = uint32;

        public:
            explicit Deep_Inline ArchetypeBitField(ECRegistry* registry);
            Deep_Inline ArchetypeBitField(const ArchetypeBitField&) = default;
            Deep_Inline ArchetypeBitField(ArchetypeBitField&&) noexcept = default;

            Deep_Inline ArchetypeBitField& operator=(const ArchetypeBitField&) = default;
            Deep_Inline ArchetypeBitField& operator=(ArchetypeBitField&&) noexcept = default;

            Deep_Inline bool HasComponent(ComponentId component) const;

            ArchetypeBitField& AddComponent(ComponentId component);

            ArchetypeBitField& RemoveComponent(ComponentId component);

        private:
            ECRegistry* const registry;

            std::vector<Type> bits;
        };

    public:
        class Archetype;

        struct ArchetypeDesc {
            friend class Archetype;

        public:
            explicit Deep_Inline ArchetypeDesc(ECRegistry* registry);
            Deep_Inline ArchetypeDesc(const ArchetypeDesc&) = default;
            Deep_Inline ArchetypeDesc(ArchetypeDesc&&) noexcept = default;

            Deep_Inline ArchetypeDesc& operator=(const ArchetypeDesc&) = default;
            Deep_Inline ArchetypeDesc& operator=(ArchetypeDesc&&) noexcept = default;

            Deep_Inline bool HasComponent(ComponentId component) const;

            ArchetypeDesc& AddComponent(ComponentId component) &;
            Deep_Inline ArchetypeDesc&& AddComponent(ComponentId component) &&;

            ArchetypeDesc& RemoveComponent(ComponentId component) &;
            Deep_Inline ArchetypeDesc&& RemoveComponent(ComponentId component) &&;

            ECRegistry* const registry;

        private:
            ArchetypeBitField type;

            // NOTE(randomuserhi): The order of which components are added determines the memory layout of the components in
            //                     the chunk.
            // NOTE(randomuserhi): Does not include tags.
            std::vector<ComponentDesc> layout;
        };

        class Archetype final : NonCopyable {
            friend class ECDB;

            // NOTE(randomuserhi): Handle edge case where archetype is of size 0 (entity with just tags, no components)
            //                     Pretty much just don't allocate any memory at all and the EntityPtr just points to this
            //                     archetype with nullptr chunk.

        public:
            static const size_t chunkAllocSize = 16384;
            static_assert(chunkAllocSize > sizeof(void*), "chunkAllocSize must be larger than a pointer.");
            static_assert(Deep::IsPowerOf2(chunkAllocSize), "chunkAllocSize must be a power of 2.");

            static constexpr const size_t chunkSize = chunkAllocSize - sizeof(void*);
            struct Chunk {
                char data[chunkSize];
                Chunk* next = nullptr;
            };
            static_assert(std::is_standard_layout<Chunk>(), "Chunk must be of standard layout.");
            static_assert(sizeof(Chunk) == chunkAllocSize, "Size of chunk does not match chunkAllocSize.");

            explicit Deep_Inline Archetype(ECDB* database);
            Archetype(ECDB* database, ArchetypeDesc&& description);
            ~Archetype();

            size_t GetComponentOffset(ComponentId component) const;

            // Describes the archetype (type + layout)
            const ArchetypeDesc description;

        private:
            ECDB* const database;

            size_t entitiesPerChunk = 0;

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
            // - Number of components is low
            //
            // NOTE(randomuserhi): Does not include tags since they dont contribute any data.
            // NOTE(randomuserhi): `ids` holds ComponentId + 1 as the value 0 is used to denote an empty bucket slot
            //                     during initialization.
            // TODO(randomuserhi): Use a single allocated block (where ids and offsets are adjacent) instead of 2 vectors
            std::vector<ComponentId> ids;
            std::vector<size_t> offsets;

            std::unordered_map<ComponentId, Archetype*> addMap;

            // TODO(randomuserhi): Needs to somehow associate to the same archetypes as the addMap
            //                     (2 different paths can result in the same archetype now)
            //                     Probably just iterate database->archetypes to find matching type
            //                     (number of archs is low enough to linear search)
            std::unordered_map<ComponentId, Archetype*> removeMap;
        };

    private:
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
        explicit Deep_Inline ECDB(ECRegistry* registry);
        ~ECDB();

        ECDB::Entt Entity();

        ECDB::Archetype& GetArchetype(ComponentId* components, size_t numComponents);

    private:
        ECRegistry* const registry;

        std::vector<Archetype*> archetypes;

        Archetype rootArchetype;

        uint32 firstFreeItemInNewPage = 0;

        EntityPage::Storage* firstFree = nullptr;

        EntityPage* entityPages = nullptr;
    };

    using Entt = ECDB::Entt;
} // namespace Deep

#include "ECDB.inl"