#pragma once

#include "../../Deep.h"
#include "../NonCopyable.h"

#include <atomic>
#include <mutex>

namespace Deep {
    // Lockless (As long as no new page allocation is required) implementation of a fixed size free list (https://en.wikipedia.org/wiki/Free_list)
    // Page size must be a power of 2 due to bit mask operations
    // 
    // Implementation based on Jolt: https://github.com/jrouwe/JoltPhysics/blob/master/Jolt/Core/FixedSizeFreeList.h
    template<typename T>
    class FixedSizeFreeList : NonCopyable {
    private:
        struct ItemStorage {
            T item;
            std::atomic<uint32> nextFreeItem;
        };
        static_assert(alignof(ItemStorage) == alignof(T), "ItemStorage is not aligned properly");

    public:
        explicit inline FixedSizeFreeList(uint32 maxItems, uint32 pageSize);
        inline ~FixedSizeFreeList();

        template <typename... Parameters>
        inline uint32 ConstructItem(Parameters &&... parameters);

        inline void DestructItem(uint32 itemIndex);
        inline void DestructItem(T* item);

        static const uint32 invalidItemIndex = 0xffffffff;
        static const int32 itemStorageSize = sizeof(ItemStorage);

        struct Batch {
            uint32 firstItemIndex = invalidItemIndex;
            uint32 lastItemIndex = invalidItemIndex;
            uint32 size = 0;
        };

        inline void AddItemToBatch(Batch& batch, uint32 itemIndex);

        inline void DestructItemBatch(Batch& batch);

    private:
        inline const ItemStorage& GetStorage(uint32 index) const;
        inline ItemStorage& GetStorage(uint32 index);

        uint32 pageSize;
        uint32 pageShift;
        uint32 itemMask;
        uint32 numPages;
        uint32 size;
        ItemStorage** pages = nullptr;

        alignas(DEEP_CACHE_LINE_SIZE) std::mutex pageMutex;

        std::atomic<uint32> allocTag; // https://en.wikipedia.org/wiki/Compare-and-swap https://en.wikipedia.org/wiki/ABA_problem#Tagged_state_reference
        std::atomic<uint32> firstFreeItemAndTag;
        std::atomic<uint32> firstFreeItemInNewPage;
    };
}

#include "FixedSizeFreeList.inl"