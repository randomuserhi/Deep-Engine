#include "./Semaphore.h" // not needed btw
#include "../Math.h"

#ifdef DEEP_PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace Deep {
    template<ptrdiff_t leastMaxValue>
    const int32 Semaphore<leastMaxValue>::GetValue() const {
        return count;
    }

    #if __cplusplus >= 202002L
    template<ptrdiff_t leastMaxValue>
    void Semaphore<leastMaxValue>::Release(int32 update) {
        Deep_Assert(Sign(update) == 1);

        int32 oldCount = count.fetch_add(update);
        if (oldCount < 0) {
            int newCount = oldCount + update;
            int numToRelease = Min(newCount, 0) - oldCount;
            semaphore.release(numToRelease);
        }
    }

    template<ptrdiff_t leastMaxValue>
    void Semaphore<leastMaxValue>::Acquire(int32 update) {
        Deep_Assert(Sign(update) == 1);

        int32 oldCount = count.fetch_sub(update);
        int32 newCount = oldCount - update;
        if (newCount < 0) {
            int numToAquire = Min(oldCount, 0) - newCount;
            semaphore.acquire(numToAquire);
        }
    }
    #else
    template<ptrdiff_t leastMaxValue>
    Semaphore<leastMaxValue>::Semaphore() {
        #ifdef DEEP_PLATFORM_WINDOWS
        semaphore = CreateSemaphore(nullptr, 0, leastMaxValue, nullptr);
        #endif
    }

    template<ptrdiff_t leastMaxValue>
    Semaphore<leastMaxValue>::~Semaphore() {
        #ifdef DEEP_PLATFORM_WINDOWS
        ::CloseHandle(semaphore);
        #endif
    }

    template<ptrdiff_t leastMaxValue>
    void Semaphore<leastMaxValue>::Release(int32 update) {
        Deep_Assert(Sign(update) == 1);

        #ifdef DEEP_PLATFORM_WINDOWS
        int32 oldCount = count.fetch_add(update);
        if (oldCount < 0) {
            int newCount = oldCount + update;
            int numToRelease = Min(newCount, 0) - oldCount;
            ::ReleaseSemaphore(mSemaphore, num_to_release, nullptr);
        }
        #else
        std::lock_guard<std::mutex> lock(mutex);
        count += update;
        if (update > 1) {
            waitVariable.notify_all();
        } else {
            waitVariable.notify_one();
        }
        #endif
    }

    template<ptrdiff_t leastMaxValue>
    void Semaphore<leastMaxValue>::Acquire(int32 update) {
        Deep_Assert(Sign(update) == 1);

        #ifdef DEEP_PLATFORM_WINDOWS
        int32 oldCount = count.fetch_sub(update);
        int32 newCount = oldCount - update;
        if (newCount < 0) {
            int numToAquire = Min(oldCount, 0) - newCount;
            for (int i = 0; i < num_to_acquire; ++i)
                ::WaitForSingleObject(mSemaphore, INFINITE);
        }
        #else
        std::unique_lock<std::mutex> lock(mutex);
        count -= update;
        waitVariable.wait(lock, [this]() { return count >= 0; });
        #endif
    }
    #endif
}