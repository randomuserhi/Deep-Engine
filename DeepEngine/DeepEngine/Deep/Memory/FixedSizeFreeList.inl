#pragma once

#include "../Math.h"
#include "../Memory.h"
#include "../BitHelper.h"

#include <type_traits>

namespace Deep {
    template<typename T>
    FixedSizeFreeList<T>::FixedSizeFreeList(uint32 maxItems, uint32 pageSize) :
        numItems(0), firstFreeItemInNewPage(0),
        allocTag(1),
        firstFreeItemAndTag(invalidItemIndex) {

        Deep_Assert(maxItems > 0, "Must be able to contain atleast 1 item.");
        Deep_Assert(pageSize > 0 && IsPowerOf2(pageSize), "Page Size must be non-zero and a power of 2 due to bit operations.");

        // Get the least number of pages required to store the maximum number of items
        numPages = (maxItems + pageSize - 1) / pageSize;
        this->pageSize = pageSize;

        // Get the amount of shifting required to obtain the page index (high part)
        pageShift = NumTrailingZeros(pageSize);
        itemMask = pageSize - 1; // Since we guarantee that `pageSize` is a power of 2, this generates a bit mask for the bits required to store the page index (low part)

        // NOTE(randomuserhi): The logic behind `pageShift` and `itemMask` is to abuse the fact that we limit `pageSize` to a power of 2 and binary counting to split
        //                     a single incrementing index into 2 incrementing indices for 2 dimensional access `pages[page][item]`.
        //                     
        //                     Consider a 3 bit index:
        //                       4 | 2 | 1
        //                       ---------
        //                       0   0   0
        //                       0   0   1
        //                       0   1   0
        //                       0   1   1
        //                       1   0   0
        //                       1   0   1
        //                       1   1   0
        //                       1   1   1
        //                      
        //                     Since `pageSize` is always a power of 2, if the size of a page is 4 and we need 8 items (2 pages each with 4 items), then we can use the ]
        //                     highest bit as the page index (0, 1 - 2 pages) and the lower 2 bits (00, 01, 10, 11 - 4 items) for the item index. Notice how by incrementing 
        //                     a 3 bit index without accounting for pages, we get this 2 dimensional access for free.

        #ifdef DEEP_ENABLE_ASSERTS
        // Keep track of number of free items in the pool
        numFreeItems = numPages * pageSize;
        #endif

        pages = reinterpret_cast<ItemStorage**>(Malloc(numPages * sizeof(ItemStorage*)));
    }

    template<typename T>
    FixedSizeFreeList<T>::~FixedSizeFreeList() {
        if (pages != nullptr) {
            // Ensure all items are freed
            #ifdef DEEP_ENABLE_ASSERTS
            Deep_Assert(numFreeItems.load(std::memory_order_relaxed) == numPages * pageSize, "Not all items were released before memory was deallocated.");
            #endif

            // Calculate number of allocated pages from number of allocated items
            uint32 numPages = numItems / pageSize;

            // Free allocated pages
            for (uint32 page = 0; page < numPages; ++page)
                AlignedFree(pages[page]);

            Free(pages);
        }
    }

    template<typename T>
    const typename FixedSizeFreeList<T>::ItemStorage& FixedSizeFreeList<T>::GetStorage(uint32 index) const {
        Deep_Assert(index != invalidItemIndex, "Invalid index.");
        return pages[index >> pageShift][index & itemMask];
    }

    template<typename T>
    typename FixedSizeFreeList<T>::ItemStorage& FixedSizeFreeList<T>::GetStorage(uint32 index) {
        Deep_Assert(index != invalidItemIndex, "Invalid index.");
        return pages[index >> pageShift][index & itemMask];
    }

    template<typename T>
    const T& FixedSizeFreeList<T>::operator[](uint32 itemIndex) const {
        return GetStorage(itemIndex).item;
    }

    template<typename T>
    template<typename... Parameters>
    uint32 FixedSizeFreeList<T>::ConstructItem(Parameters &&... parameters) {
        for (;;) {
            uint64 firstFreeItemAndTag = this->firstFreeItemAndTag.load(std::memory_order_acquire);
            uint32 firstFree = static_cast<uint32>(firstFreeItemAndTag); // Grab the low 32 bits
            if (firstFree == invalidItemIndex) {
                // Free list is empty, take an item from the last allocated page that has not been used before

                firstFree = firstFreeItemInNewPage.fetch_add(1, std::memory_order_relaxed);
                if (firstFree >= numItems) {
                    // Allocate new page
                    std::lock_guard<std::mutex> lock(pageMutex);
                    while (firstFree >= numItems) {
                        uint32 nextPage = numItems / pageSize;
                        if (nextPage == numPages) {
                            return invalidItemIndex; // Out of space
                        }
                        pages[nextPage] = reinterpret_cast<ItemStorage*>(AlignedMalloc(pageSize * sizeof(ItemStorage), Max<size_t>(alignof(ItemStorage), DEEP_CACHE_LINE_SIZE)));
                        numItems += pageSize;
                    }
                }

                #ifdef DEEP_ENABLE_ASSERTS
                numFreeItems.fetch_sub(1, std::memory_order_relaxed);
                #endif
                ItemStorage& storage = GetStorage(firstFree);
                ::new (&storage.item) T(std::forward<Parameters>(parameters)...);
                storage.nextFreeItem.store(firstFree, std::memory_order_release);
                return firstFree;
            } else {
                // Use the first item in the free list

                uint32 newFirstFree = GetStorage(firstFree).nextFreeItem.load(std::memory_order_acquire);

                uint64 newFirstFreeItemAndTag = static_cast<uint64>(newFirstFree) + (static_cast<uint64>(allocTag.fetch_add(1, std::memory_order_relaxed)) << 32);

                if (this->firstFreeItemAndTag.compare_exchange_weak(firstFreeItemAndTag, newFirstFreeItemAndTag, std::memory_order_release)) {
                    #ifdef DEEP_ENABLE_ASSERTS
                    numFreeItems.fetch_sub(1, std::memory_order_relaxed);
                    #endif
                    ItemStorage& storage = GetStorage(firstFree);
                    ::new (&storage.item) T(std::forward<Parameters>(parameters)...);
                    storage.nextFreeItem.store(firstFree, std::memory_order_release);
                    return firstFree;
                }
            }
        }
    }

    template<typename T>
    void FixedSizeFreeList<T>::FreeItem(uint32 itemIndex) {
        Deep_Assert(itemIndex != invalidItemIndex, "Invalid index.");
        Deep_Assert(itemIndex < numItems, "Index out of bounds.");

        ItemStorage& storage = GetStorage(itemIndex);
        Deep_Assert(storage.nextFreeItem == itemIndex, "Item cannot be freed as it is already free or part of a batch to be freed.");
        storage.item.~T();

        // Add item back to free list, inserting it at the front
        for (;;) {
            uint64 firstFreeItemAndTag = this->firstFreeItemAndTag.load(std::memory_order_acquire);
            uint32 firstFree = static_cast<uint32>(firstFreeItemAndTag);

            storage.nextFreeItem.store(firstFree, std::memory_order_release);

            uint64 newFirstFreeItemAndTag = static_cast<uint64>(itemIndex) + (static_cast<uint64>(allocTag.fetch_add(1, std::memory_order_relaxed)) << 32);

            if (this->firstFreeItemAndTag.compare_exchange_weak(firstFreeItemAndTag, newFirstFreeItemAndTag, std::memory_order_release)) {
                // Free successful
                #ifdef DEEP_ENABLE_ASSERTS
                numFreeItems.fetch_add(1, std::memory_order_relaxed);
                #endif
                return;
            }
        }
    }

    template<typename T>
    void FixedSizeFreeList<T>::FreeItem(const T* item) {
        uint32 index = reinterpret_cast<const ItemStorage*>(item)->nextFreeItem.load(std::memory_order_relaxed);
        Deep_Assert(item == &GetStorage(index).item, "Item cannot be freed as it is either invalid, already free or part of a batch to be freed.");
        FreeItem(index);
    }

    template<typename T>
    void FixedSizeFreeList<T>::AddItemToBatch(Batch& batch, uint32 itemIndex) {
        #ifdef DEEP_ENABLE_ASSERTS
        if (batch.owner == nullptr) {
            batch.owner = this;
        }
        Deep_Assert(batch.owner == this, "Cannot add an item that belongs to a different list.");
        #endif

        Deep_Assert(batch.size != static_cast<uint32>(-1), "Batch has already been freed.");

        // Reset next index to invalid
        std::atomic<uint32>& nextFreeItem = GetStorage(itemIndex).nextFreeItem;
        Deep_Assert(nextFreeItem.load(std::memory_order_relaxed) == itemIndex, "Item has already been destructed and is already in the free list.");
        nextFreeItem.store(invalidItemIndex, std::memory_order_release);

        // Link object in batch to free, adding it to the end of the linked list
        if (batch.firstItemIndex == invalidItemIndex) {
            batch.firstItemIndex = itemIndex;
        } else {
            GetStorage(batch.lastItemIndex).nextFreeItem.store(itemIndex, std::memory_order_release);
        }
        batch.lastItemIndex = itemIndex;
        ++batch.size;
    }

    template<typename T>
    void FixedSizeFreeList<T>::FreeBatch(Batch& batch) {
        if (batch.firstItemIndex != invalidItemIndex) {
            if
                #if __cplusplus >= 201703L
                constexpr
                #endif 
                (!std::is_trivially_destructible<T>()) {
                // Loop and destruct each item
                uint32 itemIndex = batch.firstItemIndex;
                do {
                    ItemStorage& storage = GetStorage(itemIndex);
                    storage.item.~T();
                    itemIndex = storage.nextFreeItem.load(std::memory_order_relaxed);
                } while (itemIndex != invalidItemIndex);
            }

            ItemStorage& storage = GetStorage(batch.lastItemIndex);
            for (;;) {
                uint64 firstFreeItemAndTag = this->firstFreeItemAndTag.load(std::memory_order_acquire);
                uint32 firstFree = static_cast<uint32>(firstFreeItemAndTag);

                storage.nextFreeItem.store(firstFree, std::memory_order_release);

                uint64 newFirstFreeItemAndTag = static_cast<uint64>(batch.firstItemIndex) + (static_cast<uint64>(allocTag.fetch_add(1, std::memory_order_relaxed)) << 32);

                if (this->firstFreeItemAndTag.compare_exchange_weak(firstFreeItemAndTag, newFirstFreeItemAndTag, std::memory_order_release)) {
                    #ifdef DEEP_ENABLE_ASSERTS
                    numFreeItems.fetch_add(batch.size, std::memory_order_relaxed);

                    batch.size = static_cast<uint32>(-1);
                    #endif
                    return;
                }
            }
        }
}
}