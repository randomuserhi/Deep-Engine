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
        threads = reinterpret_cast<std::thread*>(Malloc(numThreads * sizeof(std::thread)));
        for (int32 i = 0; i < numThreads; ++i) {
            ::new (threads + i) std::thread([this, i] { ThreadMain(i); });
        }
    }

    void JobSystem::StopThreads() {
        if (threads == nullptr) return;

        running = false;
        semaphore.Release(numThreads);

        for (int32 i = 0; i < numThreads; ++i) {
            std::thread& thread = threads[i];
            if (thread.joinable()) {
                thread.join();
            }
        }

        Free(threads);
    }

    void JobSystem::ThreadMain(int32 id) {

    }

    JobHandle JobSystem::CreateJob(JobFunction jobFunction, uint32 numDependencies) {
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

        return JobHandle{ job };
    }
}

// Class Job
namespace Deep {
    JobSystem::Job::Job(JobSystem* jobSystem, JobFunction jobFunction, uint32 numDependencies) :
        jobSystem(jobSystem), jobFunction(jobFunction), numDependencies(numDependencies) {

    }
}