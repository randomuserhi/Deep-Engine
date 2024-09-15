#pragma once

namespace Deep {
    Deep_Inline void* Malloc(size_t size);
    Deep_Inline void* Realloc(void* old, size_t size);
    Deep_Inline void Free(void* block);
    Deep_Inline void* AlignedMalloc(size_t size, size_t alignment);
    Deep_Inline void AlignedFree(void* pointer);
}

#include "./Memory.inl"