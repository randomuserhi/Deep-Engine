/**
 * Job System
 */

#pragma once

#include "../../Deep.h"
#include "../NonCopyable.h"
#include "../BitHelper.h"
#include "./semaphore.h"
#include "../Memory/FixedSizeFreeList.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

/// TODO:
/// [x] JobSystem(size_t numThreads)
/// [ ] JobHandle job = JobSystem.CreateJob(std::function<void()> job, uint32 numDependencies)
/// [ ] job.Acquire(1)
/// [ ] job.Release(1)
/// [ ] JobSystem.Enqueue(job);
/// [ ] Barrier barrier = JobSystem.CreateBarrier();
/// [ ] barrier.WaitAll()
/// [ ] JobSystem.Wait(job) -> inline creates a barrier and calls barrier.WaitAll()
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

        private:
            // Job system that owns this job
            const JobSystem* jobSystem;

            // Function the job executes
            JobFunction jobFunction;

            // Number of dependencies left before this job can execute
            std::atomic<uint32> numDependencies;
        };

    public:
        class JobHandle {
            // TODO(randomuserhi): ...
        };

        // Constructors
        explicit JobSystem(size_t numThreads, uint32 maxJobs);
        ~JobSystem();

        JobHandle CreateJob(JobFunction jobFunction, uint32 numDependencies);
        void Enqueue(JobHandle job);

    private:
        void StartThreads();
        void StopThreads();
        void ThreadMain(size_t id);

        Semaphore<INT_MAX> semaphore;

        FixedSizeFreeList<Job> jobs;

        std::thread* threads = nullptr;

        size_t numThreads;

        std::atomic<bool> running = true;
    };

    using JobHandle = JobSystem::JobHandle;
}
