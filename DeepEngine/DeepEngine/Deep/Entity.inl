#pragma once

namespace Deep {
    ComponentDesc::ComponentDesc(ComponentId id, size_t size, size_t alignment, const char* name) :
        id(id), size(size), alignment(alignment), name(name) {}
} // namespace Deep