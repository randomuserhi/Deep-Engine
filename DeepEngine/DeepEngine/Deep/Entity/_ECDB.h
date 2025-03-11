#pragma once

// TODO(randomuserhi): Remove this include, its unnecessary but VS is being stupid
#include <Deep/Bithelper.h>
#include <Deep/Entity.h>

#include <Deep/NonCopyable.h>

#include <unordered_map>
#include <vector>

// TODO(randomuserhi):
// - API to trade chunks between archetypes that share the same layout
//   (This allows fast bulk moving of entities between archetypes)
// - Refactor chunk API for better iteration

namespace Deep {
    struct ArchetypeBitField {
        friend struct std::hash<Deep::ArchetypeBitField>;

    private:
        using Type = uint32;
        static const size_t bitsPerType = 32;

    public:
        explicit Deep_Inline ArchetypeBitField(ECRegistry* registry);
        Deep_Inline ArchetypeBitField(const ArchetypeBitField&) = default;
        Deep_Inline ArchetypeBitField(ArchetypeBitField&&) noexcept = default;

        Deep_Inline ArchetypeBitField& operator=(const ArchetypeBitField&) = default;
        Deep_Inline ArchetypeBitField& operator=(ArchetypeBitField&&) noexcept = default;

        friend bool operator!=(const ArchetypeBitField& a, const ArchetypeBitField& b);
        friend bool operator==(const ArchetypeBitField& a, const ArchetypeBitField& b);

        Deep_Inline bool HasComponent(ComponentId component) const;

        ArchetypeBitField& AddComponent(ComponentId component);

        ArchetypeBitField& RemoveComponent(ComponentId component);

    private:
        ECRegistry* const registry;

        std::vector<Type> bits;
    };
} // namespace Deep

template<>
struct std::hash<Deep::ArchetypeBitField> {
    std::size_t operator()(const Deep::ArchetypeBitField& k) const {
        std::size_t hash = 0;

        for (size_t i = 0; i < k.bits.size(); ++i) {
            if (k.bits[i] != 0) {
                hash ^= std::hash<Deep::ArchetypeBitField::Type>()(k.bits[i]) ^ (i * 0xabefcedf);
            }
        }

        return hash;
    }
};

namespace Deep {

    // TODO(randomuserhi): Asserts and debug mode memory safety (counting number of delete calls etc...)

    class ECDB final : NonCopyable {
    private:
        struct EntityPtr;

    public:
        struct Entt {
            explicit Deep_Inline Entt(ECDB* database, EntityPtr* ptr);

            Deep_Inline operator ECDB::EntityPtr* const() const;

            Deep_Inline Entt& AddComponent(ComponentId component);

            Deep_Inline Entt& RemoveComponent(ComponentId component);

        private:
            ECDB* const database;

            EntityPtr* const ptr;
        };

    public:
        class Archetype;

        struct ArchetypeDesc {
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

            ArchetypeBitField type;

            // NOTE(randomuserhi): The order of which components are added determines the memory layout of the components in
            //                     the chunk.
            // NOTE(randomuserhi): Does not include tags.
            std::vector<ComponentDesc> layout;
        };

        class Archetype final : NonCopyable {
            friend class ECDB;

        public:
            using Chunk = uint8;

            struct Metadata {
                EntityPtr* entt;
            };
            static_assert(std::is_standard_layout<Metadata>(), "Metadata must be of standard layout.");
            static_assert(sizeof(Metadata) % alignof(Metadata) == 0,
                          "Size of Metadata must be a multiple of its alignment for proper packing.");

            struct ComponentOffset {
                size_t offset;
                size_t size;
            };

        public:
            explicit Deep_Inline Archetype(ECDB* database);
            Archetype(ECDB* database, ArchetypeDesc&& description);
            ~Archetype();

            ComponentOffset GetComponentOffset(ComponentId component) const;

            Deep_Inline const Metadata& GetMeta(EntityPtr* entity) const;

            Deep_Inline size_t GetChunkSize(Chunk* chunk) const;

            static Deep_Inline const Metadata* GetMetaList(Chunk* chunk);

            static Deep_Inline void* GetCompList(Chunk* chunk, ComponentOffset offset);

            template<typename T>
            static Deep_Inline T* GetCompList(Chunk* chunk, ComponentOffset offset);

            Deep_Inline void* GetComponent(EntityPtr* entity, ComponentOffset offset) const;

            Deep_Inline void* GetComponent(EntityPtr* entity, ComponentId component) const;

            template<typename T>
            Deep_Inline T& GetComponent(EntityPtr* entity, ComponentId component) const;

            template<typename T>
            Deep_Inline T& GetComponent(EntityPtr* entity, ComponentOffset offset) const;

            ECDB::Entt Entity();

            void Move(EntityPtr* entity);

            Deep_Inline void Remove(EntityPtr* entity);

            Deep_Inline size_t size() const;

            // Describes the archetype (type + layout)
            const ArchetypeDesc description;

            Deep_Inline Chunk* chunks() const;

            Deep_Inline Chunk* GetNextChunk(Chunk* chunk) const;

        private:
            Deep_Inline void SetNextChunk(Chunk* chunk, Chunk* next);

            void Deallocate(EntityPtr* entity);

            ECDB* const database;

            // NOTE(randomuserhi): The same as the size of the data-region of the chunk
            //                     The pointer to the next chunk appears right after the data-region
            //                     and the data region is aligned properly.
            const size_t chunkSize;

            size_t entitiesPerChunk = chunkSize / sizeof(Metadata);

            // Number of entities occupying this archetype
            size_t numEntities = 0;

            size_t firstFreeItemInNewChunk = 0;

            Chunk* firstFree = nullptr;

            // NOTE(randomuserhi): The singly linked list is organised backwards (from tail -> head)
            Chunk* tail = nullptr;

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
            std::vector<ComponentOffset> offsets;

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
            // TODO(randomuserhi): For concurrent access, each Storage in entityLookup should be aligned to
            //                     DEEP_CACHE_LINE_SIZE

            struct Storage {
                EntityPtr ptr;
                Storage* next;
            };

            static const size_t pageSize = 127;

            // NOTE(randomuserhi): These should remain uninitialized until allocated
            Storage entityLookup[pageSize];

            EntityPage* next = nullptr;

        private:
            char _padding[sizeof(Storage) - sizeof(EntityPage*)];
        };

    public:
        explicit Deep_Inline ECDB(ECRegistry* registry, size_t chunkSize = 16384);
        ~ECDB();

        ECDB::Entt Entity();

        void AddComponent(EntityPtr* entity, ComponentId component);

        void RemoveComponent(EntityPtr* entity, ComponentId component);

        Deep_Inline ECDB::Archetype& GetRootArchetype();

        ECDB::Archetype& GetArchetype(ComponentId* components, size_t numComponents);

    private:
        EntityPtr* AllocateEntity();

        ECRegistry* const registry;

        const size_t chunkSize;

        std::unordered_map<ArchetypeBitField, Archetype*> archetypes;

        Archetype* rootArchetype;

        size_t firstFreeItemInNewPage = 0;

        EntityPage::Storage* firstFree = nullptr;

        EntityPage* entityPages = nullptr;
    };

    using Entt = ECDB::Entt;
} // namespace Deep

#include "ECDB.inl"