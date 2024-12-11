/**
 * Job System
 */

#include "./JobSystem.h"

 // TODO(randomuserhi): Thread affinity ?? => seems platform dependent
 //#include <windows.h>
 //DWORD_PTR dw = ::SetThreadAffinityMask(pool.back().native_handle(), DWORD_PTR(1) << i);

// Class JobSystem
namespace Deep {
    JobSystem::JobSystem(int32 numThreads, uint32 maxJobs) :
        jobs(maxJobs, maxJobs), numThreads(numThreads) {
        Deep_Assert(Sign(numThreads) == 1, "Number of threads must be positive.");

        StartThreads();
    }

    JobSystem::~JobSystem() {
        StopThreads();
    }

    void JobSystem::StartThreads() {
        if (threads != nullptr || numThreads == 0) return;

        running = true;

        // Allocate heads for job queue per thread
        heads = reinterpret_cast<std::atomic<uint>*>(Malloc(sizeof(std::atomic<uint>) * numThreads));
        for (int i = 0; i < numThreads; ++i) {
            heads[i] = 0;
        }

        // Allocate and start threads
        threads = reinterpret_cast<std::thread*>(Malloc(numThreads * sizeof(std::thread)));
        for (int32 i = 0; i < numThreads; ++i) {
            ::new (threads + i) std::thread([this, i] { ThreadMain(i); });
        }
    }

    void JobSystem::StopThreads() {
        if (threads == nullptr) return;

        // Signal threads to stop and wake them up
        running = false;
        semaphore.Release(numThreads);

        // Wait for all threads to finish
        for (int32 i = 0; i < numThreads; ++i) {
            std::thread& thread = threads[i];
            if (thread.joinable()) {
                thread.join();
            }
        }

        // Free Threads
        Free(threads);

        // TODO(randomuserhi): Document...
        // Ensure that there are no lingering jobs in the queue, release them all
        for (uint32 head = 0; head != tail; ++head) {
            Job* job_ptr = queue[head & (queueLength - 1)].exchange(nullptr);
            if (job_ptr != nullptr) {
                job_ptr->Execute();
                job_ptr->Release();
            }
        }

        // Destroy queue heads and reset tail
        Free(heads);
        heads = nullptr;
        tail = 0;
    }

    void JobSystem::ThreadMain(int32 id) {
        // TODO(randomuserhi): Document
        std::atomic<uint32>& head = heads[id];

        while (running) {
            semaphore.Acquire();

            while (head != tail) {
                std::atomic<JobSystem::Job*>& job = queue[head & (queueLength - 1)];
                if (job.load() != nullptr) {
                    JobSystem::Job* jobPtr = job.exchange(nullptr);
                    if (jobPtr != nullptr) {
                        jobPtr->Execute();
                        jobPtr->Release(); // TODO(randomuserhi): Not sure if this should release now
                                           //                     Supposedly, if the job is released
                                           //                     it is no longer in the queue, even
                                           //                     if the job did not execute.
                    }
                }
                ++head;
            }
        }
    }

    JobSystem::JobHandle JobSystem::CreateJob(JobFunction jobFunction, uint32 numDependencies) {
        // Loop until we can construct a job in the free list
        uint32 index;
        for (;;) {
            index = jobs.ConstructItem(this, jobFunction, numDependencies);
            if (index != FixedSizeFreeList<Job>::invalidItemIndex) {
                break;
            }

            // If we can't, then stall
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        Job* const job = &jobs[index];

        // Return handle
        return JobSystem::JobHandle{ job };
    }
}

// Class Job
namespace Deep {
    JobSystem::Job::Job(JobSystem* jobSystem, JobFunction jobFunction, uint32 numDependencies) :
        jobSystem(jobSystem), jobFunction(jobFunction), numDependencies(numDependencies) {

    }
}