/**
 * ECRegistry
 */

#include "Deep/Entity.h"

namespace Deep {
    ComponentId ECRegistry::RegisterComponent(size_t size, size_t alignment, const char* name) {
        ComponentId id = static_cast<ComponentId>(lookup.size());

        // Components have the most significant bit set to 0 (are always positive)
        // Tags have the most significant bit set to 1 (are always negative)
        if (size == 0) {
            Deep_Assert(alignment == 0, "Tags (Components with size == 0) should have alignment of 0.");
            id |= tagBit;
        } else {
            Deep_Assert(alignment > 0, "An alignment < 0 is invalid.");
            Deep_Assert(Deep::IsPowerOf2(alignment), "Alignment should be a power of 2.");
            Deep_Assert(
                (size % alignment) == 0,
                "Size must be a multiple of alignment to ensure proper packing in contiguous arrays (Standard Layout).");
            id &= (~tagBit);
        }

        lookup.emplace_back(id, size, alignment, name);

        return id;
    }
} // namespace Deep