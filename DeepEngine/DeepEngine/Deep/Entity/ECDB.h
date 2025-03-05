#pragma once

#include <Deep.h>
#include <Deep/Entity.h>
#include <Deep/Entity/Archetype.h>
#include <Deep/Entity/ECRegistry.h>
#include <Deep/NonCopyable.h>

#include <unordered_map>
#include <vector>

namespace Deep {
    class ECDB final : NonCopyable {
    public:
        ECDB(ECRegistry* registry);

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
        };

        struct EntityPage {
            static const size_t pageSize = 128;

            EntityPage* next;

            EntityPtr entityLookup[pageSize];
        };

        ECRegistry* const registry;

        std::vector<Archetype> archetypes;

        std::unordered_map<ComponentId, Archetype*> archetypeMap;

        EntityPage* firstFree;

        EntityPage* entityPages;
    };
} // namespace Deep