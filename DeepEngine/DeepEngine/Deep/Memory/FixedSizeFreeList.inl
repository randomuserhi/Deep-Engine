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

        Deep_Assert(pageSize > 0 && IsPowerOf2(pageSize), "Page Size must be non-zero and a power of 2.");

        numPages = (maxItems + pageSize - 1) / pageSize;
        this->pageSize = pageSize;
        pageShift = NumTrailingZeros(pageSize);
        itemMask = pageSize - 1;

        #ifdef DEEP_ENABLE_ASSERTS
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

            uint32 numPages = numItems / pageSize;
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

    // TODO(randomuserhi): Document and work out how this works
    template<typename T>
    template<typename... Parameters>
    uint32 FixedSizeFreeList<T>::ConstructItem(Parameters &&... parameters) {
        for (;;) {
            uint64 firstFreeItemAndTag = this->firstFreeItemAndTag.load(std::memory_order_acquire);
            uint32 firstFree = static_cast<uint32>(firstFreeItemAndTag);
            if (firstFree == invalidItemIndex) {
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
        storage.item.~T();

        // Add item back to free list
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
        FreeItem(index);
    }

    template<typename T>
    void FixedSizeFreeList<T>::AddItemToBatch(Batch& batch, uint32 itemIndex) {
        Deep_Assert(GetStorage(itemIndex).nextFreeItem.load(std::memory_order_relaxed) == itemIndex, "Item is already in free list."); // Trying to add a object to the batch that is already in a free list
        Deep_Assert(batch.size != static_cast<uint32>(-1), "Batch has already been freed."); // Trying to reuse a batch that has already been freed

        // Link object in batch to free
        if (batch.firstItemIndex == invalidItemIndex)
            batch.firstItemIndex = itemIndex;
        else
            GetStorage(batch.lastItemIndex).nextFreeItem.store(itemIndex, std::memory_order_release);
        batch.lastItemIndex = itemIndex;
        ++batch.size;
    }

    template<typename T>
    void FixedSizeFreeList<T>::FreeItemBatch(Batch& batch) {
        if (batch.firstItemIndex != invalidItemIndex) {
            if
                #if __cplusplus >= 201703L
                constexpr
                #endif 
                (!std::is_trivially_destructible<T>()) {
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