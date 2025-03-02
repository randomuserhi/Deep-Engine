/**
 * Job System
 */

#pragma once

#include "../../Deep.h"
#include "../BitHelper.h"
#include "../Memory/Reference.h"
#include "../Memory/FixedSizeFreeList.h"
#include "../Memory/PooledFixedSizeFreeList.h"
#include "../NonCopyable.h"
#include "./semaphore.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace Deep {
    class JobSystem final : private NonCopyable {
    public:
        using JobFunction = std::function<void()>;

    private:
        class Barrier; // Forward declaration

        class Job final : private NonCopyable {
            friend class Barrier;

        public:
            Job(JobSystem* jobSystem, JobFunction jobFunction, uint32 numDependencies);

            // Acquire a reference to this job
            Deep_Inline void Acquire();

            // Release a reference to this job
            Deep_Inline void Release();

            // Set dependencies
            Deep_Inline void SetDependencies(uint32 count);

            // Add a dependency
            Deep_Inline void AddDependency(uint32 count);

            // Remove a dependency and queues the job when count == 0
            Deep_Inline void RemoveDependency(uint32 count);

            // Returns true if the job has 0 dependencies, and thus can be executed
            Deep_Inline bool CanBeExecuted() const;

            // Execute jobFunction, if it fails to execute, return the number of dependencies / current state
            Deep_Inline uint32 Execute();

            // Returns true if the job has executed
            Deep_Inline bool IsDone() const;

        private:
            // Try assign a barrier to this job
            Deep_Inline bool SetBarrier(Barrier* barrier);

            // Job system that owns this job
            JobSystem* const jobSystem;

            // Function the job executes
            JobFunction jobFunction;

            // Pointer to barrier that this job is associated with
            std::atomic<intptr_t> barrier = 0;

            // Value of `barrier` when the barrier has been triggered
            static constexpr intptr_t barrierDoneState = ~intptr_t(0);

            // Number of references to this job. Used for reference counting to free the job.
            std::atomic<uint32> referenceCount;

            // Number of dependencies left before this job can execute
            // NOTE(randomuserhi): Also stores the job state, refer to `executingState` and `doneState`
            std::atomic<uint32> numDependencies;

            // Value of `numDependencies` when job is executing
            static constexpr uint32 executingState = 0xe0e0e0e0;

            // Value of `numDependencies` when job is done
            static constexpr uint32 doneState = 0xd0d0d0d0;
        };

    public:
        class JobHandle final : private Ref<Job> {
            friend class JobSystem;

        public:
            // Inherit constructors
            using Ref<Job>::Ref;

            inline JobHandle() = default;
            inline JobHandle(const JobHandle& handle) = default;
            inline JobHandle(JobHandle&& handle) noexcept;

            inline JobHandle& operator=(const JobHandle& ref) = default;
            inline JobHandle& operator=(JobHandle&& ref) noexcept = default;

            Deep_Inline void AddDependency(uint32 count) const;
            Deep_Inline void RemoveDependency(uint32 count) const;

            Deep_Inline bool IsDone() const;

        private:
            // Inherit GetPtr
            using Ref<Job>::GetPtr;
        };

    private:
        class Barrier final : private NonCopyable {
            friend class JobSystem;
            friend class Job;

        public:
            Barrier();
            ~Barrier();

            void AddJob(const JobHandle& job);

            // Wait for all jobs in the barrier to complete, while waiting, execute jobs that are part of this barrier on the
            // current thread
            void Wait();

            // Returns true if there are no jobs in the barrier
            Deep_Inline bool IsEmpty() const;

            // Acquire a reference to this barrier
            Deep_Inline void Acquire();

            // Release a reference to this barrier
            Deep_Inline void Release();

        private:
            // Called by a job to mark it as finished within this barrier
            Deep_Inline void OnJobFinished(Job* job);

            // Job system that owns this job
            JobSystem* jobSystem = nullptr;

            // TODO(randomuserhi): More optimal barrier job queue...

            // Job queue for the barrier
            static constexpr uint32 maxJobs = 2048;
            static_assert(IsPowerOf2(maxJobs), "Maximum job count must be a power of 2 due to bit operations.");
            std::atomic<Job*> jobs[maxJobs];

            // First job that can be read from job queue
            alignas(DEEP_CACHE_LINE_SIZE) std::atomic<uint32> readIndex = 0;

            // First job that can be written from job queue
            alignas(DEEP_CACHE_LINE_SIZE) std::atomic<uint32> writeIndex = 0;

            // Number of times the semaphore has been released, the barrier needs to acquire the semaphore this many times
            std::atomic<int> numToAcquire = 0;

            // Semaphore used by finishing jobs to signal the barrier that they're done
            Semaphore<INT_MAX> semaphore;

            // Number of references to this job. Used for reference counting to free the job.
            std::atomic<uint32> referenceCount;
        };

    public:
        class BarrierHandle final : private Ref<Barrier> {
            friend class JobSystem;

        public:
            // Inherit constructors
            using Ref<Barrier>::Ref;

            inline BarrierHandle() = default;
            inline BarrierHandle(const BarrierHandle& handle) = default;
            inline BarrierHandle(BarrierHandle&& handle) noexcept;

            Deep_Inline void AddJob(const JobHandle& job) const;

            // Wait for all jobs in the barrier to complete, while waiting, execute jobs that are part of this barrier on the
            // current thread
            Deep_Inline void Wait() const;

            // Returns true if there are no jobs in the barrier
            Deep_Inline bool IsEmpty() const;

        private:
            // Inherit GetPtr
            using Ref<Barrier>::GetPtr;
        };

    public:
        // Constructors
        explicit JobSystem(int32 numThreads, uint32 maxJobs, uint32 maxBarriers);
        ~JobSystem();

        // Creates a job and queues it
        JobHandle Enqueue(JobFunction jobFunction, uint32 numDependencies = 0);

        // Obtain a barrier
        BarrierHandle AcquireBarrier();

    private:
        void StartThreads();
        void StopThreads();
        void ThreadMain(int32 id);

        // Free a job back into free list
        Deep_Inline void FreeJob(Job* job);

        // Release a barrier back into pool
        Deep_Inline void ReleaseBarrier(Barrier* barrier);

        // Get the head that has processed the least jobs
        Deep_Inline uint32 GetMinHead();

        // Internally add a job to the job queue
        void QueueJobInternal(Job* job);

        FixedSizeFreeList<Job> jobs;

        std::thread* threads = nullptr;

        const int32 numThreads;

        // Job queue
        //
        // TODO(randomuserhi): Document how this job queue works
        static constexpr uint32 queueLength = 1024;
        static_assert(IsPowerOf2(queueLength), "Queue length must be a power of 2 due to bit operations.");
        std::atomic<Job*> queue[queueLength];

        // Per executing thread, the head of the job queue
        std::atomic<uint32>* heads = nullptr;

        // Tail (write end) of the job queue
        alignas(DEEP_CACHE_LINE_SIZE) std::atomic<uint32> tail = 0;

        Semaphore<INT_MAX> semaphore;

        PooledFixedSizeFreeList<Barrier> barriers;

        std::atomic<bool> running = true;
    };

    using Job = JobSystem::JobHandle;
    using Barrier = JobSystem::BarrierHandle;
} // namespace Deep

#include "./JobSystem.inl"