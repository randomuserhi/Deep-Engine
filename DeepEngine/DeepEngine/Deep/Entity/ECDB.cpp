/**
 * ECDB
 */

#include <Deep/Math.h>
#include <Deep/Entity.h>
#include <Deep/Memory.h>

namespace Deep {
    // TODO(randomuserhi): Make inline
    ECDB::ECDB(ECRegistry* registry) :
        registry(registry) {}

    ECDB::~ECDB() {
        // Free archetypes
        for (size_t i = 0; i < archetypes.size(); ++i) {
            delete archetypes[i];
        }

        // Free entity pages
        while (entityPages != nullptr) {
            EntityPage* temp = entityPages;
            entityPages = entityPages->next;
            delete entityPages;
        }

        firstFree = nullptr;
    }

    ECDB::Ent ECDB::Entity() {
        // TODO(randomuserhi): Support Concurrency (lock-free)

        EntityPage::Storage* storage;

        if (firstFree != nullptr) {
            // Take first item of free list

            storage = firstFree;
            firstFree = firstFree->next;
        } else {
            // Free list is empty

            if (entityPages != nullptr) {
                // Page allocated

                if (firstFreeItemInNewPage < EntityPage::pageSize) {
                    // Allocated page has space

                    storage = &(entityPages->entityLookup[firstFreeItemInNewPage++]);
                } else {
                    // Allocated page is full

                    EntityPage* newPage = new EntityPage{ entityPages };
                    entityPages = newPage;

                    storage = &(entityPages->entityLookup[0]);

                    firstFreeItemInNewPage = 1;
                }
            } else {
                // No pages allocated

                entityPages = new EntityPage{ nullptr };

                storage = &(entityPages->entityLookup[firstFreeItemInNewPage++]);
            }
        }

        storage->next = nullptr;

        EntityPtr& ptr = storage->ptr;
        ptr.archetype = nullptr;
        ptr.chunk = nullptr;

        return ECDB::Ent{ this, &ptr };
    }
} // namespace Deep

namespace Deep {
    void ECDB::ArchetypeType::AddComponent(ComponentId component) {
        Deep_Assert(registry->Has(component), "ComponentId does not exist in registry.");
        Deep_Assert(!HasComponent(component), "Type already contains the given component.");

        uint32 i = component / sizeof(Type);
        while (i >= bits.size()) {
            bits.emplace_back(0u);
        }
        bits[i] |= (1u << (component % sizeof(Type)));

        components.push_back(registry->Get(component));
    }

    void ECDB::ArchetypeType::RemoveComponent(ComponentId component) {
        Deep_Assert(HasComponent(component), "Type does not have the given component.");

        uint32 i = component / sizeof(Type);
        if (i < bits.size()) {
            bits[i] &= ~(1u << (component % sizeof(Type)));
        }

        // NOTE(randomuserhi): linear search should be sufficient since component type count is often low
        for (size_t i = 0; i < components.size(); ++i) {
            if (components[i].id == component) {
                components.erase(components.begin() + i);
                break;
            }
        }
    }

    size_t ECDB::ArchetypeType::Size() {
        if (components.size() == 0) return 0;

        size_t size = components[0].size;

        for (size_t i = 1; i < components.size(); ++i) {
            ComponentDesc& comp = components[i];
            size_t padding = comp.alignment - (size % comp.alignment);
            size += padding + comp.size;
        }

        return size;
    }

    size_t ECDB::ArchetypeType::Alignment() {
        if (components.size() == 0) return 0;

        size_t alignment = components[0].alignment;

        for (size_t i = 1; i < components.size(); ++i) {
            ComponentDesc& comp = components[i];
            alignment = Deep::Max(alignment, comp.alignment);
        }

        return alignment;
    }
} // namespace Deep

namespace Deep {
    ECDB::Archetype::Archetype(ECDB* database, ArchetypeType type) :
        database(database), type(type), chunkSize(type.Size()), chunkAlignment(type.Alignment()) {}

    ECDB::Archetype::~Archetype() {
        while (chunks != nullptr) {
            Archetype::Chunk* temp = chunks;
            chunks = chunks->next;
            Free(chunks);
        }

        firstFree = nullptr;
    }
} // namespace Deep