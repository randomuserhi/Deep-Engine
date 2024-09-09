#pragma once

#include "./FixedSizeFreeList.h" // Not needed btw

#include "../Memory.h"
#include "../BitHelper.h"

namespace Deep {
    template<typename T>
    FixedSizeFreeList<T>::FixedSizeFreeList(uint32 maxItems, uint32 pageSize) :
        size(0), firstFreeItemInNewPage(0),
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

    template<typename T>
    template<typename... Parameters>
    uint32 FixedSizeFreeList<T>::ConstructItem(Parameters &&... parameters) {
        for (;;) {

        }
    }
}