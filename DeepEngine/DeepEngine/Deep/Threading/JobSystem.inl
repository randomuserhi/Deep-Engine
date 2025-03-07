#pragma once

// Class JobSystem
namespace Deep {
    void JobSystem::FreeJob(JobSystem::Job* job) {
        jobs.FreeItem(job);
    }

    void JobSystem::ReleaseBarrier(JobSystem::Barrier* barrier) {
        barriers.ReleaseItem(barrier);
    }
} // namespace Deep

// Class Job
namespace Deep {
    void JobSystem::Job::Acquire() {
        referenceCount.fetch_add(1, std::memory_order_relaxed);
    }

    void JobSystem::Job::Release() {
        // Releasing a reference must use release semantics...
        if (referenceCount.fetch_sub(1, std::memory_order_release) == 1) {
            // ... so that we can use acquire to ensure that we see any updates from other threads that released a ref before
            // freeing the job
            std::atomic_thread_fence(std::memory_order_acquire);
            jobSystem->FreeJob(this);
        }

        // NOTE(randomuserhi): For TSAN no atomic_thread_fence can be used, so the below alternative is required
        // TODO(randomuserhi): TSAN #define flag
        /*
        if (referenceCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            jobSystem->jobs.FreeItem(this);
        }
        */
    }

    void JobSystem::Job::SetDependencies(uint32 count) {
#ifdef DEEP_ENABLE_ASSERTS
        uint32 state = numDependencies.load(std::memory_order_relaxed);
        Deep_Assert(state != doneState && state != executingState,
                    "Cannot set dependencies of a completed / executing job.");
#endif

        numDependencies.store(count, std::memory_order_relaxed);
    }

    void JobSystem::Job::AddDependency(uint32 count) {
#ifdef DEEP_ENABLE_ASSERTS
        uint32 state = numDependencies.load(std::memory_order_relaxed);
        Deep_Assert(state != doneState && state != executingState,
                    "Cannot set dependencies of a completed / executing job.");
#endif

        numDependencies.fetch_add(count, std::memory_order_relaxed);
    }

    void JobSystem::Job::RemoveDependency(uint32 count) {
#ifdef DEEP_ENABLE_ASSERTS
        uint32 state = numDependencies.load(std::memory_order_relaxed);
        Deep_Assert(state != doneState && state != executingState,
                    "Cannot set dependencies of a completed / executing job.");
#endif

        uint32 oldCount = numDependencies.fetch_sub(count, std::memory_order_release);
        uint32 newCount = oldCount - count;
        Deep_Assert(oldCount > newCount, "Logical Error. Removed more dependencies than there were available.");

        // Queue the job if dependencies == 0
        if (newCount == 0) {
            jobSystem->QueueJobInternal(this);
        }
    }

    uint32 JobSystem::Job::Execute() {
        // Transition job to executing state
        uint32 state = 0;
        if (!numDependencies.compare_exchange_strong(state, executingState, std::memory_order_acquire)) {
            // If the job has already executed or has dependencies left, early return
            return state;
        }

        // Run the Job
        jobFunction();

        // Exchange barrier pointer to the done state, so no barrier is set after job has completed
        intptr_t barrierPtr = barrier.load(std::memory_order_relaxed);
        for (;;) {
            if (barrier.compare_exchange_weak(barrierPtr, barrierDoneState, std::memory_order_relaxed)) break;
        }
        Deep_Assert(barrierPtr != barrierDoneState,
                    "Barrier should not have been in done state. The job may have run twice?");

        // Transition job to done state
        state = executingState;
        numDependencies.compare_exchange_strong(state, doneState, std::memory_order_relaxed);
        Deep_Assert(state == executingState, "The job should have been in executing state. The job may have run twice?");

        // Notify barrier that a job has completed
        if (barrierPtr != 0) {
            reinterpret_cast<Barrier*>(barrierPtr)->OnJobFinished(this);
        }

        return doneState;
    }

    bool JobSystem::Job::IsDone() const {
        return numDependencies.load(std::memory_order_relaxed) == doneState;
    }

    bool JobSystem::Job::CanBeExecuted() const {
        return numDependencies.load(std::memory_order_relaxed) == 0;
    }

    bool JobSystem::Job::SetBarrier(Barrier* barrier) {
        intptr_t barrierPtr = 0;
        if (this->barrier.compare_exchange_strong(barrierPtr, reinterpret_cast<intptr_t>(barrier),
                                                  std::memory_order_relaxed)) {
            return true;
        }
        Deep_Assert(barrierPtr == barrierDoneState, "A job can only belong to 1 barrier");
        return false;
    }
} // namespace Deep

// Class JobHandle
namespace Deep {
    JobSystem::JobHandle::JobHandle(JobHandle&& handle) noexcept :
        Ref<Job>(std::move(handle)) {}

    void JobSystem::JobHandle::AddDependency(uint32 count = 1) const {
        Deep_Assert(ptr != nullptr, "JobHandle is nullptr.");
        ptr->AddDependency(count);
    }

    void JobSystem::JobHandle::RemoveDependency(uint32 count = 1) const {
        Deep_Assert(ptr != nullptr, "JobHandle is nullptr.");
        ptr->RemoveDependency(count);
    }

    bool JobSystem::JobHandle::IsDone() const {
        return ptr->IsDone();
    }
} // namespace Deep

// Class Barrier
namespace Deep {
    bool JobSystem::Barrier::IsEmpty() const {
        return readIndex == writeIndex;
    }

    void JobSystem::Barrier::OnJobFinished(Job* job) {
        // NOTE(randomuserhi): Since we do not pop the job off the barrier queue, if Wait() has not been called, the barrier
        //                     can fill up and eventually become full even if all jobs are completed.

        semaphore.Release();
    }

    void JobSystem::Barrier::Acquire() {
        referenceCount.fetch_add(1, std::memory_order_relaxed);
    }

    void JobSystem::Barrier::Release() {
        Deep_Assert(jobSystem != nullptr, "Barrier was not assigned a job system.");

        // Releasing a reference must use release semantics...
        if (referenceCount.fetch_sub(1, std::memory_order_release) == 1) {
            // ... so that we can use acquire to ensure that we see any updates from other threads that released a ref before
            // freeing the job
            std::atomic_thread_fence(std::memory_order_acquire);
            jobSystem->ReleaseBarrier(this);
        }

        // NOTE(randomuserhi): For TSAN no atomic_thread_fence can be used, so the below alternative is required
        // TODO(randomuserhi): TSAN #define flag
        /*
        if (referenceCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            jobSystem->jobs.FreeItem(this);
        }
        */
    }
} // namespace Deep

#include <iostream>

// Class BarrierHandle
namespace Deep {
    JobSystem::BarrierHandle::BarrierHandle(BarrierHandle&& handle) noexcept :
        Ref<Barrier>(std::move(handle)) {}

    void JobSystem::BarrierHandle::AddJob(const JobHandle& job) const {
        ptr->AddJob(job);
    }

    void JobSystem::BarrierHandle::Wait() const {
        ptr->Wait();
    }

    bool JobSystem::BarrierHandle::IsEmpty() const {
        return ptr->IsEmpty();
    }
} // namespace Deep