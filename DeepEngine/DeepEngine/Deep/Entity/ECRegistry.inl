#pragma once

#include <type_traits>

// Class ECRegistry
namespace Deep {
    template<typename T>
    ComponentId ECRegistry::RegisterComponent(const char* name) {
        static_assert(std::is_trivially_constructible<T>(), "Component types must be trivially constructable.");

        return RegisterComponent(sizeof(T), alignof(T), name);
    }

    ComponentId ECRegistry::RegisterTag(const char* name) {
        return RegisterComponent(0, 0, name);
    }

    template<typename T>
    ComponentId ECRegistry::RegisterTag(const char* name) {
        static_assert(std::is_empty<T>(), "Tag types must be empty.");

        return RegisterTag(name);
    }

    const ComponentDesc& ECRegistry::Get(ComponentId id) const {
        Deep_Assert(Has(id), "Component does not exist in registry.");
        return lookup[id];
    }

    bool ECRegistry::Has(ComponentId id) const {
        Deep_Assert(id >= 0, "ComponentId's should be positive.");
        return id < lookup.size();
    }
} // namespace Deep

// Class ECStaticRegistry
namespace Deep {
    ECStaticRegistry::ECStaticRegistry(ECRegistry* registry) :
        registry(registry) {}

    template<typename T>
    ComponentId ECStaticRegistry::RegisterComponent() {
        std::type_index idx = std::type_index(typeid(T));
        Deep_Assert(lookup.find(idx) == lookup.end(), "Component already exists.");

        return lookup[idx] = registry->RegisterComponent<T>(typeid(T).name());
    }

    template<typename T>
    ComponentId ECStaticRegistry::RegisterTag() {
        std::type_index idx = std::type_index(typeid(T));
        Deep_Assert(lookup.find(idx) == lookup.end(), "Tag already exists.");

        return lookup[idx] = registry->RegisterTag<T>(typeid(T).name());
    }

    template<typename T>
    ComponentId ECStaticRegistry::Get() {
        std::type_index idx = std::type_index(typeid(T));
        Deep_Assert(lookup.find(idx) != lookup.end(), "Component or tag was not registered yet.");

        return lookup[idx];
    }
} // namespace Deep