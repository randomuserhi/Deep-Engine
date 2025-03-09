/**
 * ECDB
 */

#include <Deep/Math.h>
#include <Deep/Entity.h>
#include <Deep/Memory.h>

namespace Deep {
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

    ECDB::Entt ECDB::Entity() {
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

        return ECDB::Entt{ this, &ptr };
    }
} // namespace Deep

namespace Deep {
    void ECDB::ArchetypeBitField::AddComponent(ComponentId component) {
        Deep_Assert(registry->Has(component), "ComponentId does not exist in registry.");
        Deep_Assert(!HasComponent(component), "Type already contains the given component.");

        uint32 i = component / sizeof(Type);
        while (i >= bits.size()) {
            bits.emplace_back(0u);
        }
        bits[i] |= (1u << (component % sizeof(Type)));
    }

    void ECDB::ArchetypeBitField::RemoveComponent(ComponentId component) {
        Deep_Assert(HasComponent(component), "Type does not have the given component.");

        uint32 i = component / sizeof(Type);
        if (i < bits.size()) {
            bits[i] &= ~(1u << (component % sizeof(Type)));
        }
    }
} // namespace Deep

namespace Deep {
    void ECDB::ArchetypeDesc::AddComponent(ComponentId component) {
        Deep_Assert(!HasComponent(component), "Type already contains the given component.");

        type.AddComponent(component);

        if (ECRegistry::IsComponent(component)) {
            layout.push_back(registry->Get(component));
        }
    }

    void ECDB::ArchetypeDesc::RemoveComponent(ComponentId component) {
        Deep_Assert(HasComponent(component), "Type does not contain the given component.");

        type.RemoveComponent(component);

        if (ECRegistry::IsComponent(component)) {
            // NOTE(randomuserhi): A linear search to remove the component is fast enough
            //                     since the number of components is often low.
            for (size_t i = 0; i < layout.size(); ++i) {
                if (layout[i].id == component) {
                    layout.erase(layout.begin() + i);
                    break;
                }
            }
        }
    }
} // namespace Deep

namespace Deep {
    ECDB::Archetype::Archetype(ECDB* database, ArchetypeDesc&& desc) :
        database(database),
        description(std::move(desc)),
        ids(description.layout.size()),
        offsets(description.layout.size()) {
        for (size_t i = 0; i < description.layout.size(); ++i) {
            const ComponentDesc& comp = description.layout[i];
            Deep_Assert(ECRegistry::IsComponent(comp.id), "Layout should only consist of components, not tags.");

            // Add member offset entry
            size_t bucket = comp.id % description.layout.size();
            while (ids[bucket] != 0) {
                ++bucket;
            };
            Deep_Assert(comp.id + 1 != 0,
                        "Storing an id of 0 in the bucket is invalid as 0 represents an empty bucket slot.");
            ids[bucket] = comp.id + 1;
            offsets[bucket] = chunkSize;

            // Size
            size_t padding = comp.alignment - (chunkSize % comp.alignment);
            chunkSize += padding + comp.size;

            // Alignment
            chunkAlignment = Deep::Max(chunkAlignment, comp.alignment);
        }
    }

    ECDB::Archetype::~Archetype() {
        while (chunks != nullptr) {
            Archetype::Chunk* temp = chunks;
            chunks = chunks->next;
            Free(chunks);
        }

        firstFree = nullptr;
    }

    size_t ECDB::Archetype::GetComponentOffset(ComponentId component) {
        size_t bucket = component % description.layout.size();
        while (ids[bucket] != component + 1) {
            ++bucket;
        };
        return offsets[bucket];
    }
} // namespace Deep