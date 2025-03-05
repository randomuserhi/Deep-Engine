/**
 * ECRegistry
 */

#include "Deep/Entity/ECRegistry.h"

namespace Deep {
    ComponentId ECRegistry::RegisterComponent(size_t size, size_t alignment, const char* name) {
        int id = lookup.size();
        lookup.emplace_back(ComponentDesc{ size, alignment, name });
        return id;
    }
} // namespace Deep