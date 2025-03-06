#pragma once

#include <Deep.h>
#include <Deep/Entity.h>
#include <Deep/Entity/Archetype.h>
#include <Deep/Entity/ECRegistry.h>
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
            Ent(EntityPtr* ptr) :
                ptr(ptr) {}; // TODO(randomuserhi): move somewhere else

        private:
            EntityPtr* ptr;
        };

    private:
        class Archetype final : NonCopyable {
        public:
            struct Chunk {
                Chunk* next = nullptr;
            };

            Archetype(ECDB* database);
            ~Archetype();

        private:
            ECDB* const database;

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

        std::vector<Archetype> archetypes;

        std::unordered_map<ComponentId, Archetype*> archetypeMap;

        uint32 firstFreeItemInNewPage = 0;

        EntityPtr* firstFree = nullptr;

        EntityPage* entityPages = nullptr;
    };

    using Ent = ECDB::Ent;
} // namespace Deep