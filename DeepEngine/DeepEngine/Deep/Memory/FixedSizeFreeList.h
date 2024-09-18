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
    // The item index is a 32 bit value split into 2 parts (low and high). The high part of the index
    // represents the page index and the low part represents the item index within said page.
    // The number of bits in the high and low part are determined by the maximum number of items
    // the list can contain alongside the number of items per page. This can be done as the size
    // of each page is limited to being a power of 2.
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

        // Lockless construct a new Item of type T, parameters are passed to constructor
        template <typename... Parameters>
        inline uint32 ConstructItem(Parameters &&... parameters);

        // Lockless destruct an item and return it to free pool
        inline void FreeItem(uint32 itemIndex);
        inline void FreeItem(const T* item);

        static const uint32 invalidItemIndex = 0xffffffff;
        static const int32 itemStorageSize = sizeof(ItemStorage);

        // Represents a batch of items to destruct
        struct Batch {
            #ifdef DEEP_ENABLE_ASSERTS
            void* owner = nullptr;
            #endif
            uint32 firstItemIndex = invalidItemIndex;
            uint32 lastItemIndex = invalidItemIndex;
            uint32 size = 0;
        };

        // Adds an item to a batch to destruct later
        inline void AddItemToBatch(Batch& rwBatch, uint32 itemIndex);

        // Lockless destruct a batch of items
        inline void FreeItemBatch(Batch& rwBatch);

        // Get an item by index
        Deep_Inline const T& operator[](uint32 itemIndex) const;

    private:
        // Get item storage via index
        inline const ItemStorage& GetStorage(uint32 index) const;
        inline ItemStorage& GetStorage(uint32 index);

        // Number of items per page
        uint32 pageSize;

        // Number of bits to shift item index to the right to get page number (High part of index)
        uint32 pageShift;

        // Bit mask to apply to an item index to get page number (Low part of index)
        uint32 itemMask;

        // Number of usable pages
        uint32 numPages;

        // Total number of items allocated (regardless of constructed or not)
        uint32 numItems;

        // Array of pages and items
        ItemStorage** pages = nullptr;

        // Mutex for allocating a new page if storage runs out
        // NOTE(randomuserhi): Aligned to cache size to prevent false sharing with the constants used to index into the list via `operator []`
        alignas(DEEP_CACHE_LINE_SIZE) std::mutex pageMutex;

        #ifdef DEEP_ENABLE_ASSERTS
        // Number of items in free list.
        // When asserts are enabled, ensures that all items are destructed when the list destructs
        std::atomic<uint32> numFreeItems;
        #endif

        // Counter used to make the first free item pointer, `firstFreeItemAndTag`, update with every CAS to avoid ABA
        // CAS - https://en.wikipedia.org/wiki/Compare-and-swap
        // ABA - https://en.wikipedia.org/wiki/ABA_problem#Tagged_state_reference
        std::atomic<uint32> allocTag;

        // TODO(randomuserhi)
        std::atomic<uint64> firstFreeItemAndTag;

        // TODO(randomuserhi)
        std::atomic<uint32> firstFreeItemInNewPage;
    };
}

#include "FixedSizeFreeList.inl"