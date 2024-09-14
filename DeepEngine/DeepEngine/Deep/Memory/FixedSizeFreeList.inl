#pragma once

#include "./FixedSizeFreeList.h" // Not needed btw

#include "../Math.h"
#include "../Memory.h"
#include "../BitHelper.h"

namespace Deep {
    template<typename T>
    FixedSizeFreeList<T>::FixedSizeFreeList(uint32 maxItems, uint32 pageSize) :
        numItems(0), firstFreeItemInNewPage(0),
        allocTag(1),
        firstFreeItemAndTag(invalidItemIndex) {

        Deep_Assert(pageSize > 0 && IsPowerOf2(pageSize));

        numPages = (maxItems + pageSize - 1) / pageSize;
        this->pageSize = pageSize;
        pageShift = NumTrailingZeros(pageSize);
        itemMask = pageSize - 1;

        pages = reinterpret_cast<ItemStorage**>(Malloc(numPages * sizeof(ItemStorage*)));
    }

    template<typename T>
    const FixedSizeFreeList<T>::ItemStorage& FixedSizeFreeList<T>::GetStorage(uint32 index) const {
        return pages[index >> pageShift][index & itemMask];
    }

    template<typename T>
    FixedSizeFreeList<T>::ItemStorage& FixedSizeFreeList<T>::GetStorage(uint32 index) {
        return pages[index >> pageShift][index & itemMask];
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
                    std::lock_guard lock(pageMutex);
                    while (firstFree >= numItems) {
                        uint32 nextPage = numItems / pageSize;
                        if (nextPage == numPages) {
                            return invalidItemIndex; // Out of space
                        }
                        pages[nextPage] = reinterpret_cast<ItemStorage*>(AlignedMalloc(pageSize * sizeof(ItemStorage), Max<size_t>(alignof(ItemStorage), DEEP_CACHE_LINE_SIZE)));
                        numItems += pageSize;
                    }
                }

                ItemStorage& storage = GetStorage(firstFree);
                ::new (&storage.item) T(std::forward<Parameters>(parameters)...);
                storage.nextFreeItem.store(firstFree, std::memory_order_release);
                return firstFree;
            } else {
                uint32 newFirstFree = GetStorage(firstFree).nextFreeItem.load(std::memory_order_acquire);

                uint64 newFirstFreeItemAndTag = static_cast<uint64>(newFirstFree) + (static_cast<uint64>(allocTag.fetch_add(1, std::memory_order_relaxed)) << 32);

                if (this->firstFreeItemAndTag.compare_exchange_weak(firstFreeItemAndTag, newFirstFreeItemAndTag, std::memory_order_release)) {
                    ItemStorage& storage = GetStorage(firstFree);
                    ::new (&storage.item) T(std::forward<Parameters>(parameters)...);
                    storage.nextFreeItem.store(firstFree, std::memory_order_release);
                    return firstFree;
                }
            }
        }
    }

    template<typename T>
    void FixedSizeFreeList<T>::DestructItem(uint32 itemIndex) {
        Deep_Assert(itemIndex != invalidItemIndex);
        Deep_Assert(itemIndex < numItems);

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
                return;
            }
        }
    }

    template<typename T>
    void FixedSizeFreeList<T>::DestructItem(T* item) {
        uint32 index = reinterpret_cast<ItemStorage*>(item)->nextFreeItem.load(std::memory_order_relaxed);
        DestructItem(index);
    }

    template<typename T>
    void FixedSizeFreeList<T>::AddItemToBatch(Batch& batch, uint32 itemIndex) {

    }

    template<typename T>
    void FixedSizeFreeList<T>::DestructItemBatch(Batch& batch) {

    }
}