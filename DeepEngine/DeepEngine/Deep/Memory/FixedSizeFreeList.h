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
    // Implementation based on Jolt: https://github.com/jrouwe/JoltPhysics/blob/master/Jolt/Core/FixedSizeFreeList.h
    template<typename T>
    class FixedSizeFreeList final : private NonCopyable {
    public:
        // Represents a batch of items to destruct
        struct Batch {
            friend class FixedSizeFreeList<T>;

        private:
#ifdef DEEP_ENABLE_ASSERTS
            // NOTE(randomuserhi): Until the first item is added, the batch does not belong to a particular list
            void* owner = nullptr;
#endif
            uint32 firstItemIndex = invalidItemIndex;
            uint32 lastItemIndex = invalidItemIndex;
            uint32 size = 0;
        };

    private:
        // NOTE(randomuserhi): Align each item storage to prevent false sharing on concurrent writes to
        //                     adjacent items
        struct alignas(DEEP_CACHE_LINE_SIZE) ItemStorage {
            friend class FixedSizeFreeList<T>;

        private:
            T item;

            // When the item is freed (or in the process of being freed as a batch) this will contain the index to the next
            // free item (The free list)
            // When an item is in use it will contain the item's index, such that when freed via
            // `T*`, the index is known to properly handle it
            std::atomic<uint32> nextFreeItem;
        };
        static_assert(alignof(ItemStorage) == (DEEP_CACHE_LINE_SIZE > alignof(T) ? DEEP_CACHE_LINE_SIZE : alignof(T)),
                      "ItemStorage is not aligned properly");

    public:
        FixedSizeFreeList() = delete;
        explicit inline FixedSizeFreeList(uint32 maxItems, uint32 pageSize);
        inline ~FixedSizeFreeList();

        // Lockless construct a new Item of type T, parameters are passed to constructor
        template<typename... Parameters>
        inline uint32 AllocItem(Parameters&&... parameters);

        // Lockless destruct an item and return it to free pool
        inline void FreeItem(uint32 itemIndex);

        // Lockless destruct an item and return it to free pool
        // NOTE(randomuserhi): Undefined behaviour if called on an item that is part of a batch which is yet to be destructed
        //                     as the `nextFreeItem` value for this item will no longer be its index, but the next item in
        //                     the batch linked list.
        inline void FreeItem(const T* item);

        static const uint32 invalidItemIndex = 0xffffffff;
        static const int32 itemStorageSize = sizeof(ItemStorage);

        // Adds an item to a batch to destruct later
        // NOTE(randomuserhi): This operation is not thread-safe in regards to `rwBatch`.
        //                     If `rwBatch` is changed by another thread as this operation runs,
        //                     it results in undefined behaviour.
        inline void AddItemToBatch(Batch& rwBatch, uint32 itemIndex);

        // Lockless destruct a batch of items
        // NOTE(randomuserhi): This operation is not thread-safe in regards to `rwBatch`.
        //                     If `rwBatch` is changed by another thread as this operation runs,
        //                     it results in undefined behaviour.
        inline void FreeBatch(Batch& rwBatch);

        // Get an item by index
        Deep_Inline T& operator[](uint32 itemIndex) const;

    private:
        // Get item storage via index
        inline ItemStorage& GetStorage(uint32 index) const;

        // Number of items per page
        uint32 pageSize;

        // Number of bits to shift item index to the right to get page number (High part of index)
        uint32 pageShift;

        // Bit mask to apply to an item index to get item number (Low part of index)
        uint32 itemMask;

        // Number of usable pages
        uint32 numPages;

        // Current number of items allocated in memory, should be a multiple of `pageSize`
        // NOTE(randomuserhi): Not the number of items constructed via `AllocItem`
        uint32 numItems;

        // Array of pages and items
        ItemStorage** pages = nullptr;

        // Mutex for allocating a new page if storage runs out
        // NOTE(randomuserhi): Aligned to cache size to prevent false sharing with the constants used to index into the list
        // via `operator []`
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

        // Head of the free list
        // NOTE(randomuserhi): The low 32 bits represent the item index and the high 32 bits is the current allocTag for CAS
        // and avoiding ABA
        std::atomic<uint64> firstFreeItemAndTag;

        // The item index of the first free item in the last allocated page
        std::atomic<uint32> firstFreeItemInNewPage;
    };
} // namespace Deep

#include "FixedSizeFreeList.inl"