#pragma once

#include "../../Deep.h"
#include "../NonCopyable.h"
#include "../ECS.h"

#include <vector>
#include <unordered_map>
#include <typeindex>

namespace Deep {
    class ECRegistry final : NonCopyable {
    private:
        struct ComponentDesc;

    public:
        ComponentId RegisterComponent(size_t size, size_t alignment, const char* name = nullptr);

        template<typename T>
        Deep_Inline ComponentId RegisterComponent(const char* name = nullptr);

        Deep_Inline ComponentId RegisterTag(const char* name = nullptr);

        template<typename T>
        Deep_Inline ComponentId RegisterTag(const char* name = nullptr);

        Deep_Inline const ComponentDesc& operator[](ComponentId id) const;

    private:
        struct ComponentDesc {
            const size_t size;
            const size_t alignment;
            const char* name;
        };

        std::vector<ComponentDesc> lookup;
    };

    class ECStaticRegistry final : NonCopyable {
    public:
        Deep_Inline ECStaticRegistry(ECRegistry* registry);

        template<typename T>
        Deep_Inline ComponentId RegisterComponent();

        template<typename T>
        Deep_Inline ComponentId RegisterTag();

        template<typename T>
        Deep_Inline ComponentId Get();

    private:
        ECRegistry* const registry;
        std::unordered_map<std::type_index, ComponentId> lookup;
    };
} // namespace Deep

#include "./ECRegistry.inl"