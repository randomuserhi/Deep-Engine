/**
 * ECRegistry
 */

#include "Deep/Entity.h"

namespace Deep {
    ComponentId ECRegistry::RegisterComponent(size_t size, size_t alignment, const char* name) {
        ComponentId id = static_cast<ComponentId>(lookup.size());
        lookup.emplace_back(id, size, alignment, name);
        return id;
    }
} // namespace Deep