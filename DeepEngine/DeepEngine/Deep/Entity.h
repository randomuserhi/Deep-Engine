#pragma once

#include <Deep.h>

namespace Deep {
    using ComponentId = int32;

    struct ComponentDesc {
        // TODO(randomuserhi): move somewhere else
        Deep_Inline ComponentDesc(ComponentId id, size_t size, size_t alignment, const char* name);

        ComponentId id;
        size_t size;
        size_t alignment;
        const char* name;
    };
} // namespace Deep

#include "Entity.inl"

#include "Entity/_ECRegistry.h"
#include "Entity/_ECDB.h"