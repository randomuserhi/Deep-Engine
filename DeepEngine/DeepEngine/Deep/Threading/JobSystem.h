/**
 * Job System
 */

#pragma once

#include "../../Deep.h"
#include "../NonCopyable.h"
#include "../BitHelper.h"
#include "./semaphore.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

/// TODO:
/// [ ] JobSystem(size_t numThreads)
/// [ ] JobHandle job = JobSystem.Enqueue(std::function<void()> job) -> enqueues a job returning a handle
/// [ ] job.Completed() -> returns if job has completed
/// [ ] job.Wait() -> blocks current thread until the job completes
/// [ ] JobHandle job = JobSystem.After(JobHandle target, std::function<void()> job) -> creates a job that executes after the target job is completed
/// [ ] job.Then(std::function<void()> job) -> creates a job that executes after the current job is completed (Calls the above function)
/// [ ] JobHandle all = JobSystem.All(JobHandle job1, JobHandle job2, ...) -> creates a job that completes when all passed jobs are completed
/// [ ] Efficient allocation of semaphore leastMaxValue instead of just INT_MAX
/// [ ] Jobs can lockless create jobs and queue them as well
/// 
/// Things to Note
/// - Construction of condition_variable and mutex being slow

#ifndef DEEP_JOB_SYSTEM_SEMAPHORE_LEAST_MAX_VALUE
#define DEEP_JOB_SYSTEM_SEMAPHORE_LEAST_MAX_VALUE INT_MAX
#endif

namespace Deep {
    class JobSystem : NonCopyable {
    public:
        using JobFunction = std::function<void()>;

        explicit JobSystem(size_t numThreads);
        ~JobSystem();

    private:
        Semaphore<DEEP_JOB_SYSTEM_SEMAPHORE_LEAST_MAX_VALUE> semaphore;
        std::atomic<bool> running = true;
    };

    class JobHandle {

    };
}