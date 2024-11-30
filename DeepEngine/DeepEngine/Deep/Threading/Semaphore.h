#pragma once

#include "../../Deep.h"
#if __cplusplus >= 202002L
#include <semaphore>
#endif
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace Deep {
    // Local implementation of semaphore as on some systems default implementation is slow
    //
    // Implementation based on Jolt: https://github.com/jrouwe/JoltPhysics/blob/master/Jolt/Core/Semaphore.h
    template<ptrdiff_t leastMaxValue>
    class Semaphore {
        #if __cplusplus >= 202002L

    public:
        Semaphore() = default;
        ~Semaphore() = default;

        // undefined for negative values
        void Release(int32 update = 1);
        void Acquire(int32 update = 1);

        Deep_Inline const int32 GetValue() const;

    private:
        // Align to cache line to prevent false sharing when writing to count
        // We increment count for every release, to acquire we decrement the count
        // If the count is negative we know that we are waiting on the actual semaphore
        alignas(DEEP_CACHE_LINE_SIZE) std::atomic<int32> count{ 0 };
        std::counting_semaphore<leastMaxValue> semaphore{ 0 };

        #else

    public:
        Semaphore();
        ~Semaphore();

        // undefined for negative values
        void Release(int32 update = 1);
        void Acquire(int32 update = 1);

        Deep_Inline const int GetValue() const;

    private:
        #ifdef DEEP_PLATFORM_WINDOWS
        // On windows we use a semaphore object since it is more efficient than a lock and a condition variable

        // Align to cache line to prevent false sharing when writing to count
        // We increment count for every release, to acquire we decrement the count
        // If the count is negative we know that we are waiting on the actual semaphore
        alignas(DEEP_CACHE_LINE_SIZE) std::atomic<int32> count{ 0 };
        void* semaphore; ///< The semaphore is an expensive construct so we only acquire/release it if we know that we need to wait/have waiting threads
        #else
        // Emulate a semaphore using a mutex, condition variable and count on non-windows platforms
        std::mutex mutex;
        std::condition_variable waitVariable;
        int32 count = 0;
        #endif

        #endif
    };
}

#include "./Semaphore.inl"