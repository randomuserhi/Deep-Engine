/**
 * ECS
 */

#pragma once

#include "../Deep.h"
#include <concurrent_unordered_map.h>

namespace Deep {
    using namespace concurrency;

    class ECS {
    public:
    private:
        concurrent_unordered_map<uint64, uint8> hierarchy;
        concurrent_unordered_map<uint64, uint8> archetypes;
        // root
    };
}

// API Specification
// - Custom allocator -> arena allocator (https://dev.to/aboss123/advanced-c-arena-allocation-3580)
// - Runtime support -> registering components at runtime (modding support)
// - Compile support -> when creating entities, runtime does not need to traverse archetype tree as constexpr evaluates it at compile time
// - Access entities in a thread safe manner
// - Very fast query operation to get set of entities by archetype
// - Attach empty archetypes (tags) to entities
//   - These should have 0 cost in terms of memory
//   - Minimal fragmentation -> dont really think about this for now tbh
// - Keep a referemce to a single entity (entity ID lookup) -> thread safe access
// - O(1) Add, Remove components / tags from entities [sparse set datastructure]
// - O(1) Add, Remove entities