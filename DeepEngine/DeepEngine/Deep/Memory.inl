#pragma once

#include <cstring>

namespace Deep {
    void* Malloc(size_t size) {
        Deep_Assert(size > 0, "Size must be greater than 0.");
        return std::malloc(size);
    }

    void* Realloc(void* old, size_t size) {
        Deep_Assert(size > 0, "Size must be greater than 0.");
        return std::realloc(old, size);
    }

    void Free(void* block) {
        std::free(block);
    }

    void* AlignedMalloc(size_t size, size_t alignment) {
        Deep_Assert(size > 0 && alignment > 0, "Size and alignment must be greater than 0.");

#ifdef DEEP_PLATFORM_WINDOWS
        return _aligned_malloc(size, alignment);
#else
        void* block = nullptr;
        // NOTE(randomuserhi): Suppress -Wunused-result
        posix_memalign(&block, alignment, size);
        return block;
#endif
    }

    void AlignedFree(void* block) {
#ifdef DEEP_PLATFORM_WINDOWS
        _aligned_free(block);
#else
        free(block);
#endif
    }

    void* Memcpy(void* dest, void* src, size_t size) {
        return std::memcpy(dest, src, size);
    }
} // namespace Deep