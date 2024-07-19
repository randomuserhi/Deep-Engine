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