/**
 * Job System
 */

#pragma once

#include "../../Deep.h"
#include "../BitHelper.h"
#include "../Memory/FixedSizeFreeList.h"
#include "../NonCopyable.h"
#include "./semaphore.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

/// TODO:
/// [x] JobSystem(size_t numThreads)
/// [x] JobHandle job = JobSystem.Enqueue(std::function<void()> job, uint32 numDependencies)
/// [ ] job.AddDependency(1)
/// [ ] job.RemoveDependency(1)
/// [ ] Barrier barrier = JobSystem.CreateBarrier();
/// [ ] barrier.Wait()
///
/// DETAILS:
/// [ ] Job Queue -> lockless
/// [ ] JobHandle reference
/// [ ] ThreadMain implementation (check job queue)
///
/// Things to Note
/// - Construction of condition_variable and mutex being slow
/// - Creating a barrier in a Job will consume a thread until that barrier is released

namespace Deep {
    class JobSystem : NonCopyable {
    private:
        class Job;

    public:
        using JobFunction = std::function<void()>;

        class JobHandle {
        public:
            Deep_Inline JobHandle() = default;
            Deep_Inline JobHandle(Job* job);
            Deep_Inline JobHandle(const JobHandle& handle);
            Deep_Inline JobHandle(JobHandle&& handle) noexcept;
            Deep_Inline ~JobHandle();

            // Assignment operators
            Deep_Inline JobHandle& operator=(Job* job);
            Deep_Inline JobHandle& operator=(const JobHandle& handle);
            Deep_Inline JobHandle& operator=(JobHandle&& handle) noexcept;

            // Casting operators
            Deep_Inline operator JobSystem::Job*() const;

            // Pointer access
            Deep_Inline Job* operator->() const;
            Deep_Inline Job& operator*() const;

            // Comparison
            Deep_Inline bool operator==(const Job* b) const;
            friend bool operator==(JobHandle& a, JobHandle& b);
            Deep_Inline bool operator!=(const Job* b) const;
            friend bool operator!=(JobHandle& a, JobHandle& b);

            Deep_Inline Job* GetPtr() const;

            Deep_Inline void AddDependency(uint32 count) const;
            Deep_Inline void RemoveDependency(uint32 count) const;

        private:
            // Wrapper for job->Acquire, noop on nullptr
            Deep_Inline void Acquire();

            // Wrapper for job->Release, noop on nullptr
            Deep_Inline void Release();

            Job* job = nullptr;
        };

        class Barrier : NonCopyable {
        public:
            void AddJob(const JobHandle& job);

            void Wait();

        private:
            static constexpr uint32 maxJobs = 2048;
            static_assert(IsPowerOf2(maxJobs), "Maximum job count must be a power of 2 due to bit operations.");
            std::atomic<Job*> jobs[maxJobs];
        };

        // Constructors
        explicit JobSystem(int32 numThreads, uint32 maxJobs);
        ~JobSystem();

        // Creates a job and queues it
        JobHandle Enqueue(JobFunction jobFunction, uint32 numDependencies = 0);

    private:
        class Job : NonCopyable {
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

            // Execute jobFunction
            Deep_Inline void Execute();

        private:
            // Job system that owns this job
            JobSystem* const jobSystem;

            // Function the job executes
            JobFunction jobFunction;

            // Number of references to this job. Used for reference counting to free the job.
            std::atomic<uint32> referenceCount;

            // Number of dependencies left before this job can execute
            std::atomic<uint32> numDependencies;
        };

        void StartThreads();
        void StopThreads();
        void ThreadMain(int32 id);

        // Free a job back into free list
        Deep_Inline void FreeJob(Job* job);

        // Get the head that has processed the least jobs
        Deep_Inline uint32 GetMinHead();

        // Internally add a job to the job queue
        void QueueJobInternal(Job* job);

        FixedSizeFreeList<Job> jobs;

        std::thread* threads = nullptr;

        const int32 numThreads;

        // Job queue
        static constexpr uint32 queueLength = 1024;
        static_assert(IsPowerOf2(queueLength), "Queue length must be a power of 2 due to bit operations.");
        std::atomic<Job*> queue[queueLength];

        // Per executing thread, the head of the job queue
        std::atomic<uint32>* heads = nullptr;

        // Tail (write end) of the job queue
        alignas(DEEP_CACHE_LINE_SIZE) std::atomic<uint32> tail = 0;

        Semaphore<INT_MAX> semaphore;

        // TODO(randomuserhi): Barriers

        std::atomic<bool> running = true;
    };

    using JobHandle = JobSystem::JobHandle;
} // namespace Deep

#include "./JobSystem.inl"