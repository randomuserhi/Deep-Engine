/**
 * ECDB
 */

#include <Deep/Entity.h>
#include <Deep/Math.h>
#include <Deep/Memory.h>

namespace Deep {
    ECDB::~ECDB() {
        // Free archetypes
        for (const auto& kv : archetypes) {
            delete kv.second;
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

                    EntityPage* newPage = new EntityPage{};
                    entityPages = newPage;

                    storage = &(entityPages->entityLookup[0]);

                    firstFreeItemInNewPage = 1;
                }
            } else {
                // No pages allocated

                entityPages = new EntityPage{};

                storage = &(entityPages->entityLookup[firstFreeItemInNewPage++]);
            }
        }

        storage->next = nullptr;

        EntityPtr& ptr = storage->ptr;
        ptr.archetype = nullptr;

        // Move the entity into the root archetype
        rootArchetype->Move(&ptr);

        return ECDB::Entt{ this, &ptr };
    }

    void ECDB::AddComponent(EntityPtr* entity, ComponentId component) {
        // TODO(randomuserhi): Thread Safety

        Deep_Assert(entity->archetype != nullptr, "Entity does not belong to any archetype.");

        Archetype* arch = entity->archetype;

        Deep_Assert(!arch->description.HasComponent(component), "Entity already has this component.");

        if (arch->addMap.find(component) != arch->addMap.end()) {
            arch->addMap[component]->Move(entity);
        } else {
            // Archetype does not exist

            ArchetypeDesc description{ arch->description };
            description.AddComponent(component);

            // Check if we already have the archetype from a different path
            if (archetypes.find(description.type) != archetypes.end()) {
                archetypes[description.type]->Move(entity);
                return;
            }

            // Create a new archetype
            Archetype* newArch = new Archetype(this, std::move(description));
            arch->addMap.emplace(component, newArch);
            newArch->removeMap.emplace(component, arch);
            archetypes.emplace(newArch->description.type, newArch);

            // Move entity to new archetype
            newArch->Move(entity);
        }
    }

    void ECDB::RemoveComponent(EntityPtr* entity, ComponentId component) {
        // TODO(randomuserhi): Thread Safety

        Deep_Assert(entity->archetype != nullptr, "Entity does not belong to any archetype.");

        Archetype* arch = entity->archetype;

        if (arch->removeMap.find(component) != arch->removeMap.end()) {
            arch->removeMap[component]->Move(entity);
        } else {
            // Archetype does not exist

            ArchetypeDesc description{ arch->description };
            description.RemoveComponent(component);

            // Check if we already have the archetype from a different path
            if (archetypes.find(description.type) != archetypes.end()) {
                archetypes[description.type]->Move(entity);
                return;
            }

            // Create a new archetype
            Archetype* newArch = new Archetype(this, std::move(description));
            arch->removeMap.emplace(component, newArch);
            newArch->addMap.emplace(component, arch);
            archetypes.emplace(newArch->description.type, newArch);

            // Move entity to new archetype
            newArch->Move(entity);
        }
    }

    ECDB::Archetype& ECDB::GetArchetype(ComponentId* components, size_t numComponents) {
        // TODO(randomuserhi): Thread Safety

        if (components == nullptr) return *rootArchetype;

        Archetype* arch = rootArchetype;
        for (size_t i = 0; i < numComponents; ++i) {
            ComponentId component = components[i];

            if (arch->addMap.find(component) != arch->addMap.end()) {
                arch = arch->addMap[component];
            } else {
                // Archetype does not exist

                ArchetypeDesc description{ arch->description };
                description.AddComponent(component);

                // Check if we already have the archetype from a different path
                if (archetypes.find(description.type) != archetypes.end()) {
                    arch = archetypes[description.type];
                    continue;
                }

                // Create a new archetype
                Archetype* newArch = new Archetype(this, std::move(description));
                arch->addMap.emplace(component, newArch);
                newArch->removeMap.emplace(component, arch);
                archetypes.emplace(newArch->description.type, newArch);

                arch = newArch;
            }
        }

        return *arch;
    }
} // namespace Deep

namespace Deep {
    bool operator!=(const ArchetypeBitField& a, const ArchetypeBitField& b) {
        size_t i = 0;
        bool smaller = a.bits.size() <= b.bits.size();
        const std::vector<ArchetypeBitField::Type>& min = smaller ? a.bits : b.bits;
        const std::vector<ArchetypeBitField::Type>& max = smaller ? b.bits : a.bits;
        for (; i < min.size(); ++i) {
            if (min[i] != max[i]) return true;
        }
        for (; i < max.size(); ++i) {
            if (max[i] != 0) return true;
        }
        return false;
    }

    bool operator==(const ArchetypeBitField& a, const ArchetypeBitField& b) {
        return !(a != b);
    }

    ArchetypeBitField& ArchetypeBitField::AddComponent(ComponentId component) {
        Deep_Assert(registry->Has(component), "ComponentId does not exist in registry.");
        Deep_Assert(!HasComponent(component), "Type already contains the given component.");

        component = ECRegistry::StripTagBit(component);

        uint32 i = component / ArchetypeBitField::bitsPerType;
        while (i >= bits.size()) {
            bits.emplace_back(0u);
        }
        bits[i] |= (1u << (component % ArchetypeBitField::bitsPerType));

        return *this;
    }

    ArchetypeBitField& ArchetypeBitField::RemoveComponent(ComponentId component) {
        Deep_Assert(HasComponent(component), "Type does not have the given component.");

        component = ECRegistry::StripTagBit(component);

        uint32 i = component / ArchetypeBitField::bitsPerType;
        if (i < bits.size()) {
            bits[i] &= ~(1u << (component % ArchetypeBitField::bitsPerType));
        }

        return *this;
    }
} // namespace Deep

namespace Deep {
    ECDB::ArchetypeDesc& ECDB::ArchetypeDesc::AddComponent(ComponentId component) & {
        Deep_Assert(!HasComponent(component), "Type already contains the given component.");

        type.AddComponent(component);

        if (ECRegistry::IsComponent(component)) {
            layout.push_back(registry->Get(component));
        }

        return *this;
    }

    ECDB::ArchetypeDesc& ECDB::ArchetypeDesc::RemoveComponent(ComponentId component) & {
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

        return *this;
    }
} // namespace Deep

namespace Deep {
    ECDB::Archetype::Archetype(ECDB* database, ArchetypeDesc&& desc) :
        database(database),
        description(std::move(desc)),
        ids(description.layout.size()),
        offsets(description.layout.size()) {

        const std::vector<ComponentDesc>& layout = description.layout;

        // If layout contains no data, then chunks only contain Metadata and there is nothing to do.
        if (layout.size() == 0) return;

#ifdef DEEP_ENABLE_ASSERTS
        for (size_t i = 0; i < layout.size(); ++i) {
            Deep_Assert(layout[i].alignment <= DEEP_CACHE_LINE_SIZE,
                        "A component has an alignment requirement larger than a cache line. This is unsupported.");
        }
#endif

        size_t packedSize = sizeof(Metadata);
        for (size_t i = 0; i < layout.size(); ++i) {
            packedSize += layout[i].size;
        }

        // Binary search to find optimal entitiesPerChunk
        size_t upperBound = Archetype::chunkSize / packedSize;
        size_t lowerBound = 0;
        entitiesPerChunk = 0;

        while (lowerBound < upperBound) {
            size_t mid = (lowerBound + upperBound) / 2;

            size_t memSize = mid * sizeof(Metadata);
            for (size_t i = 0; i < layout.size(); ++i) {
                size_t padding = (DEEP_CACHE_LINE_SIZE - (memSize % DEEP_CACHE_LINE_SIZE)) % DEEP_CACHE_LINE_SIZE;
                memSize += padding + mid * layout[i].size;
            }

            if (memSize <= chunkSize) {
                entitiesPerChunk = mid;
                lowerBound = mid + 1;
            } else {
                upperBound = mid - 1;
            }
        }

        Deep_Assert(entitiesPerChunk > 0, "Chunk should be able to fit atleast 1 entity.");

        // Compute pointer offsets for each component array
        size_t offset = entitiesPerChunk * sizeof(Metadata);
        for (size_t i = 0; i < layout.size(); ++i) {
            const ComponentDesc& comp = layout[i];
            Deep_Assert(ECRegistry::IsComponent(comp.id), "Layout should only consist of components, not tags.");

            // Apply padding as necessary
            offset += (DEEP_CACHE_LINE_SIZE - (offset % DEEP_CACHE_LINE_SIZE)) % DEEP_CACHE_LINE_SIZE;

            // Add component offset entry
            size_t bucket = comp.id % layout.size();
            while (ids[bucket] != 0) {
                ++bucket;
            };
            Deep_Assert(comp.id + 1 != 0,
                        "Storing an id of 0 in the bucket is invalid as 0 represents an empty bucket slot.");
            ids[bucket] = comp.id + 1;
            offsets[bucket] = { offset, comp.size };

            // Move to next component array
            offset += entitiesPerChunk * comp.size;
        }
    }

    ECDB::Archetype::~Archetype() {
        while (tail != nullptr) {
            Archetype::Chunk* temp = tail;
            tail = tail->next;
            Free(tail);
        }

        firstFree = nullptr;
    }

    ECDB::Archetype::ComponentOffset ECDB::Archetype::GetComponentOffset(ComponentId component) const {
        size_t bucket = component % description.layout.size();
        while (ids[bucket] != component + 1) {
            ++bucket;
        };
        return offsets[bucket];
    }

    void ECDB::Archetype::Deallocate(EntityPtr* entity) {
        // TODO(randomuserhi): Thread safety

        Deep_Assert(entity->archetype == this, "Entity does not belong to this archetype.");
        Deep_Assert(size > 0 && firstFreeItemInNewChunk > 0, "This archetype doesn't contain any entities.");
        Deep_Assert(tail != nullptr, "This archetype doesn't have any chunks allocated.");

        size_t index = firstFreeItemInNewChunk - 1;
        if (tail != entity->chunk || index != entity->index) {
            // Entity from the middle was removed, need to fill slot with last entity

            // Move component data
            for (size_t i = 0; i < description.layout.size(); ++i) {
                const ComponentDesc& comp = description.layout[i];

                size_t offset = entity->archetype->GetComponentOffset(comp.id).offset;

                char* src = tail->data + offset + comp.size * index;
                char* dest = entity->chunk->data + offset + comp.size * entity->index;
                Memcpy(dest, src, comp.size);
            }

            // Move entity pointer
            Metadata& metadata = reinterpret_cast<Metadata*>(tail->data)[index];
            EntityPtr* entt = metadata.entt;
            entt->chunk = entity->chunk;
            entt->index = entity->index;
        } else {
            // Last entity was removed, don't need to do move anything
#ifdef DEEP_ENABLE_ASSERTS
            Metadata& metadata = reinterpret_cast<Metadata*>(tail->data)[index];
            metadata.entt = nullptr;
#endif
        }

        // Remove last entity from chunk, freeing it to the free list
        if (--firstFreeItemInNewChunk == 0) {
            // Chunk no longer has any entities, free it

            Chunk* temp = tail->next;

            tail->next = firstFree;
            firstFree = tail;

            tail = temp;

            firstFreeItemInNewChunk = entitiesPerChunk;
        }

        // Decrement size
        --size;

#ifdef DEEP_ENABLE_ASSERTS
        entity->archetype = nullptr;
        entity->chunk = nullptr;
        entity->index = 0;
#endif
    }

    void ECDB::Archetype::Move(EntityPtr* entity) {
        // TODO(randomuserhi): Thread safety

        // Obtain chunk to place entity in
        Chunk* chunk;

        if (tail != nullptr) {
            // Chunks are available in chunk list

            if (firstFreeItemInNewChunk < entitiesPerChunk) {
                // Fit entity at the tail end of chunk list

                chunk = tail;
            } else {
                // Tail chunk is full, need a new chunk

                if (firstFree != nullptr) {
                    // Take chunk from free list

                    chunk = firstFree;
                    firstFree = firstFree->next;
                } else {
                    // Free list is empty, allocate a new chunk

                    chunk = new Chunk;
                }

                firstFreeItemInNewChunk = 0;

                // Append chunk to list
                chunk->next = tail;
                tail = chunk;
            }
        } else {
            // Chunk list is empty

            if (firstFree != nullptr) {
                // Take chunk from free list

                chunk = firstFree;
                firstFree = firstFree->next;
            } else {
                // Free list is empty, allocate a new chunk

                chunk = new Chunk;
            }

            firstFreeItemInNewChunk = 0;

            tail = chunk;
        }

        Deep_Assert(chunk != nullptr, "Allocated chunk should not be a nullptr.");

        // Assign metadata
        Metadata* metadata = reinterpret_cast<Metadata*>(chunk->data);
        metadata[firstFreeItemInNewChunk].entt = entity;

        if (entity->archetype != nullptr) {
            // Copy data to this archetype from old archetype component by component.

            const ArchetypeDesc& description = entity->archetype->description;
            for (size_t i = 0; i < description.layout.size(); ++i) {
                const ComponentDesc& comp = description.layout[i];

                if (this->description.HasComponent(comp.id)) {
                    char* src = entity->chunk->data + entity->archetype->GetComponentOffset(comp.id).offset
                                + comp.size * entity->index;
                    char* dest = chunk->data + GetComponentOffset(comp.id).offset + comp.size * firstFreeItemInNewChunk;
                    Memcpy(dest, src, comp.size);
                }
            }

            entity->archetype->Deallocate(entity);
        }

        // Update entity ptr
        entity->archetype = this;
        entity->chunk = chunk;
        entity->index = firstFreeItemInNewChunk;

        // Increment indices
        ++firstFreeItemInNewChunk;
        ++size;
    }
} // namespace Deep