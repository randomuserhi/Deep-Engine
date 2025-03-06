/**
 * ECDB
 */

#include <Deep/Entity/ECDB.h>
#include <Deep/Memory.h>

namespace Deep {
    ECDB::ECDB(ECRegistry* registry) :
        registry(registry) {}

    ECDB::~ECDB() {
        while (entityPages != nullptr) {
            EntityPage* temp = entityPages;
            entityPages = entityPages->next;
            delete entityPages;
        }

        firstFree = nullptr;
    }

    ECDB::Ent ECDB::Entity() {
        // TODO(randomuserhi): Support Concurrency (lock-free)

        EntityPtr* ptr;

        if (firstFree != nullptr) {
            // Take first item of free list

            ptr = firstFree;
            firstFree = firstFree->next;
        } else {
            // Free list is empty

            if (entityPages != nullptr) {
                // Page allocated

                if (firstFreeItemInNewPage < EntityPage::pageSize) {
                    // Allocated page has space

                    ptr = &(entityPages->entityLookup[firstFreeItemInNewPage++]);
                } else {
                    // Allocated page is full

                    EntityPage* newPage = new EntityPage{ entityPages };
                    entityPages = newPage;

                    ptr = &(entityPages->entityLookup[0]);

                    firstFreeItemInNewPage = 1;
                }
            } else {
                // No pages allocated

                entityPages = new EntityPage{ nullptr };

                ptr = &(entityPages->entityLookup[firstFreeItemInNewPage++]);
            }
        }

        ptr->chunk = nullptr;
        ptr->next = nullptr;

        return ECDB::Ent{ this, ptr };
    }
} // namespace Deep

namespace Deep {
    ECDB::Archetype::Archetype(ECDB* database) :
        database(database) {}

    ECDB::Archetype::~Archetype() {
        while (chunks != nullptr) {
            Archetype::Chunk* temp = chunks;
            chunks = chunks->next;
            Free(chunks);
        }

        firstFree = nullptr;
    }
} // namespace Deep