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
 /// [ ] JobHandle job = JobSystem.CreateJob(std::function<void()> job, uint32 numDependencies)
 /// [ ] job.AddDependency(1)
 /// [ ] job.RemoveDependency(1)
 /// [ ] JobSystem.Enqueue(job);
 /// [ ] JobSystem.WailAll()
 /// [ ] Barrier barrier = JobSystem.CreateBarrier();
 /// [ ] barrier.WaitAll()
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
    public:
        using JobFunction = std::function<void()>;

    private:
        class Job {
        public:
            Job(JobSystem* jobSystem, JobFunction jobFunction, uint32 numDependencies);

            // Acquire a reference to this job
            Deep_Inline void Acquire();

            // Release a reference to this job
            Deep_Inline void Release();

            // Add a dependency
            Deep_Inline void AddDependency(uint32 count);

            // Remove a dependency
            Deep_Inline bool RemoveDependency(uint32 count);

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

    public:
        class JobHandle {
        public:
            Deep_Inline JobHandle() = default;
            Deep_Inline JobHandle(Job* job);
            Deep_Inline JobHandle(const JobHandle& handle);
            Deep_Inline JobHandle(JobHandle&& handle) noexcept;

            // Assignment operators
            Deep_Inline JobHandle& operator= (Job* job);
            Deep_Inline JobHandle& operator= (const JobHandle& handle);
            Deep_Inline JobHandle& operator= (JobHandle&& handle) noexcept;

            // Casting operators
            Deep_Inline operator Job* () const;

            // Pointer access
            Deep_Inline Job* operator-> () const;
            Deep_Inline Job& operator * () const;

            // Comparison
            Deep_Inline bool operator== (const Job* b) const;
            Deep_Inline friend bool operator== (JobHandle& a, JobHandle& b);
            Deep_Inline bool operator!= (const Job* b) const;
            Deep_Inline friend bool operator!= (JobHandle& a, JobHandle& b);

            Deep_Inline Job* GetPtr() const;

            Deep_Inline void AddDependency(uint32 count) const;
            Deep_Inline void RemoveDependency(uint32 count) const;

            // TODO(randomuserhi): ...

        private:
            // Wrapper for job->Acquire
            Deep_Inline void Acquire();

            // Wrapper for job->Release
            Deep_Inline void Release();

            Job* job = nullptr;
        };

        // Constructors
        explicit JobSystem(int32 numThreads, uint32 maxJobs);
        ~JobSystem();

        JobHandle CreateJob(JobFunction jobFunction, uint32 numDependencies);
        void Enqueue(JobHandle job);

    private:
        void StartThreads();
        void StopThreads();
        void ThreadMain(int32 id);

        Deep_Inline void FreeJob(Job* job);

        FixedSizeFreeList<Job> jobs;

        std::thread* threads = nullptr;

        const int32 numThreads;

        // Job queue
        static constexpr uint32 queueLength = 1024;
        static_assert(IsPowerOf2(queueLength), "Queue length must be a power of 2 due to bit operations.");
        std::atomic<Job*> queue[queueLength];

        // Per executing thread, the head of the job queue
        std::atomic<uint>* heads = nullptr;

        // Tail (write end) of the job queue
        alignas(DEEP_CACHE_LINE_SIZE) std::atomic<uint> tail = 0;

        Semaphore<INT_MAX> semaphore;

        std::atomic<bool> running = true;
    };

    using JobHandle = JobSystem::JobHandle;
}

#include "./JobSystem.inl"