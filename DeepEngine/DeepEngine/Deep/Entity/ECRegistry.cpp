/**
 * ECRegistry
 */

#include "Deep/Entity.h"

namespace Deep {
    ComponentId ECRegistry::RegisterComponent(size_t size, size_t alignment, const char* name) {
        ComponentId id = static_cast<ComponentId>(lookup.size());
        lookup.emplace_back(id, size, alignment, name);

        // Components have the most significant bit set to 0 (are always positive)
        // Tags have the most significant bit set to 1 (are always negative)

        if (size == 0) {
            Deep_Assert(alignment == 0, "Tags (Components with size == 0) should have alignment of 0.");

            return id | tagBit;
        }

        return id & (~tagBit);
    }
} // namespace Deep