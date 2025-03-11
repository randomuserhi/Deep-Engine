#pragma once

namespace Deep {
    ECDB::Entt::Entt(ECDB* database, EntityPtr* ptr) :
        ptr(ptr), database(database) {};

    ECDB::Entt::operator ECDB::EntityPtr* const() const {
        return ptr;
    }

    ECDB::Entt& ECDB::Entt::AddComponent(ComponentId component) {
        database->AddComponent(ptr, component);
        return *this;
    }

    ECDB::Entt& ECDB::Entt::RemoveComponent(ComponentId component) {
        database->RemoveComponent(ptr, component);
        return *this;
    }
} // namespace Deep

namespace Deep {
    ECDB::ECDB(ECRegistry* registry, size_t chunkSize) :
        registry(registry), chunkSize(chunkSize) {
        rootArchetype = new Archetype(this);
        archetypes.emplace(rootArchetype->description.type, rootArchetype);
    }
} // namespace Deep

namespace Deep {
    ArchetypeBitField::ArchetypeBitField(ECRegistry* registry) :
        registry(registry) {}

    bool ArchetypeBitField::HasComponent(ComponentId component) const {
        component = ECRegistry::StripTagBit(component);

        uint32 i = component / ArchetypeBitField::bitsPerType;

        if (i < bits.size()) {
            return (bits[i] & (1u << (component % ArchetypeBitField::bitsPerType))) != 0u;
        }

        return false;
    }
} // namespace Deep

namespace Deep {
    ECDB::ArchetypeDesc::ArchetypeDesc(ECRegistry* registry) :
        registry(registry), type(registry) {}

    bool ECDB::ArchetypeDesc::HasComponent(ComponentId component) const {
        return type.HasComponent(component);
    }

    ECDB::ArchetypeDesc&& ECDB::ArchetypeDesc::AddComponent(ComponentId component) && {
        return std::move(AddComponent(component));
    };

    ECDB::ArchetypeDesc&& ECDB::ArchetypeDesc::RemoveComponent(ComponentId component) && {
        return std::move(RemoveComponent(component));
    };
} // namespace Deep

namespace Deep {
    const ECDB::Archetype::Metadata& ECDB::Archetype::GetMeta(EntityPtr* entity) const {
        Deep_Assert(entity->archetype == this, "Entity does not belong to this archetype");

        Metadata* metadata = reinterpret_cast<Metadata*>(entity->chunk);

        return metadata[entity->index];
    }

    size_t ECDB::Archetype::size() const {
        return numEntities;
    }

    ECDB::Archetype::Chunk* ECDB::Archetype::chunks() const {
        return tail;
    }

    ECDB::Archetype& ECDB::GetRootArchetype() {
        return *rootArchetype;
    }

    void ECDB::Archetype::Remove(EntityPtr* entity) {
        database->rootArchetype->Move(entity);
    }

    size_t ECDB::Archetype::GetChunkSize(Chunk* chunk) const {
        Deep_Assert(chunk != nullptr, "Chunk cannot be a nullptr.");
#ifdef DEEP_ENABLE_ASSERTS
        {
            bool found = false;
            Chunk* tail = this->tail;
            while (tail != nullptr) {
                if (tail == chunk) {
                    found = true;
                    break;
                }
                tail = GetNextChunk(tail);
            }
            Deep_Assert(found, "Chunk does not exist in this archetype.");
        }
#endif

        return chunk != tail ? entitiesPerChunk : firstFreeItemInNewChunk;
    }

    const ECDB::Archetype::Metadata* ECDB::Archetype::GetMetaList(Chunk* chunk) {
        Deep_Assert(chunk != nullptr, "Chunk cannot be a nullptr.");
        return reinterpret_cast<const ECDB::Archetype::Metadata*>(chunk);
    }

    void* ECDB::Archetype::GetCompList(Chunk* chunk, ComponentOffset offset) {
        Deep_Assert(chunk != nullptr, "Chunk cannot be a nullptr.");
        return chunk + offset.offset;
    }

    template<typename T>
    T* ECDB::Archetype::GetCompList(Chunk* chunk, ComponentOffset offset) {
        return reinterpret_cast<T*>(GetCompList(chunk, offset));
    }

    void* ECDB::Archetype::GetComponent(EntityPtr* entity, ComponentOffset offset) const {
        Deep_Assert(entity->archetype == this, "Entity does not belong to this archetype.");
        return entity->chunk + offset.offset + entity->index * offset.size;
    }

    template<typename T>
    T& ECDB::Archetype::GetComponent(EntityPtr* entity, ComponentOffset offset) const {
        return *reinterpret_cast<T*>(GetComponent(entity, offset));
    }

    void* ECDB::Archetype::GetComponent(EntityPtr* entity, ComponentId component) const {
        return GetComponent(entity, GetComponentOffset(component));
    }

    template<typename T>
    T& ECDB::Archetype::GetComponent(EntityPtr* entity, ComponentId component) const {
        return *reinterpret_cast<T*>(GetComponent(entity, GetComponentOffset(component)));
    }

    ECDB::Archetype::Chunk* ECDB::Archetype::GetNextChunk(Chunk* chunk) const {
        return *reinterpret_cast<Chunk**>(chunk + chunkSize);
    }

    void ECDB::Archetype::SetNextChunk(Chunk* chunk, Chunk* next) {
        *reinterpret_cast<Chunk**>(chunk + chunkSize) = next;
    }
} // namespace Deep

namespace Deep {
    ECDB::Archetype::Archetype(ECDB* database) :
        database(database), description(database->registry), chunkSize(database->chunkSize) {}
} // namespace Deep