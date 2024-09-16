#pragma once

#include "../../Deep.h"
#include "../NonCopyable.h"

#include <atomic>
#include <mutex>

namespace Deep {
    // Implementation of a fixed size free list (https://en.wikipedia.org/wiki/Free_list)
    // Fixed size means that it cannot create more items than the specified maxItems
    // Supports lockless construction of items (unless a new page of items needs to be allocated)
    // Supports batching items to destruct them all in a single atomic operation
    // 
    // NOTE:
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
        FixedSizeFreeList() = delete;
        explicit inline FixedSizeFreeList(uint32 maxItems, uint32 pageSize);
        inline ~FixedSizeFreeList();

        template <typename... Parameters>
        inline uint32 ConstructItem(Parameters &&... parameters);

        inline void FreeItem(uint32 itemIndex);
        inline void FreeItem(const T* item);

        static const uint32 invalidItemIndex = 0xffffffff;
        static const int32 itemStorageSize = sizeof(ItemStorage);

        // TODO(randomuserhi): Pull out of class if it can be used across templates
        struct Batch {
            uint32 firstItemIndex = invalidItemIndex;
            uint32 lastItemIndex = invalidItemIndex;
            uint32 size = 0;
        };

        // Parameter `batch` is in-out (Signal that the parameter is changed by the function)
        inline void AddItemToBatch(Batch& rwBatch, uint32 itemIndex);

        // Parameter `batch` is in-out (Signal that the parameter is changed by the function)
        inline void FreeItemBatch(Batch& rwBatch);

        inline const T& operator[](uint32 itemIndex) const;

    private:
        inline const ItemStorage& GetStorage(uint32 index) const;
        inline ItemStorage& GetStorage(uint32 index);

        uint32 pageSize;
        uint32 pageShift;
        uint32 itemMask;
        uint32 numPages;
        uint32 numItems; // total number of items allocated (regardless of constructed or not)
        ItemStorage** pages = nullptr;

        alignas(DEEP_CACHE_LINE_SIZE) std::mutex pageMutex;

        #ifdef DEEP_ENABLE_ASSERTS
        std::atomic<uint32> numFreeItems;
        #endif
        std::atomic<uint32> allocTag; // https://en.wikipedia.org/wiki/Compare-and-swap https://en.wikipedia.org/wiki/ABA_problem#Tagged_state_reference
        std::atomic<uint64> firstFreeItemAndTag;
        std::atomic<uint32> firstFreeItemInNewPage;
    };
}

#include "FixedSizeFreeList.inl"