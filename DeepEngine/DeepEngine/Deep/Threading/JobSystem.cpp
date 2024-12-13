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
                        jobPtr->Release();
                    }
                }
                ++head;
            }
        }
    }

    JobSystem::JobHandle JobSystem::Enqueue(JobFunction jobFunction, uint32 numDependencies) {
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

        // Queue the job if it has no dependencies
        if (numDependencies == 0) {
            QueueJob(job);
        }

        // Return handle
        return JobSystem::JobHandle{ job };
    }

    uint32 JobSystem::GetMinHead() {
        uint32 head = tail;
        for (size_t i = 0; i < numThreads; ++i)
            head = Deep::Min(head, heads[i].load());

        return head;
    }

    void JobSystem::QueueJob(JobSystem::Job* job) {
        // TODO(randomuserhi): Check job belongs to this job system -> assert only?
        //                     Check job has not already been executed -> maybe not to allow re-runs of the same job...
        //                     Check job is not already executing -> assert only?
        //                     Check job has 0 dependencies -> assert only?
        //                     Check we have worker threads and that the job system is running -> assert only?

        // TODO(randomuserhi): Check logic and understand what is going on...

        // Acquire a reference to the job to keep it alive in job queue
        job->Acquire();

        // Get the head that has processed the least amount of jobs
        uint32 head = GetMinHead();

        // Wake up a thread
        // NOTE(randomuserhi): We read the head outside of the loop since it involves iterating over all threads and we only need to update
        //                     it if there's not enough space in the queue.
        semaphore.Release();

        for (;;) {
            // Check if there is space in the queue
            uint oldValue = tail;
            if (oldValue - head >= queueLength) {
                // We calculated the head outside of the loop, update head (and we also need to update tail to prevent it from passing head)
                head = GetMinHead();
                oldValue = tail;

                // Second check if there's space in the queue
                if (oldValue - head >= queueLength) {
                    // Wake up all threads in order to ensure that they can clear any nullptrs they may not have processed yet
                    semaphore.Release(numThreads);

                    // Stall a little (we have to wait for other threads to update their head pointer in order for us to be able to continue)
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                    continue;
                }
            }

            // Write the job pointer if the slot is empty
            Job* expectedJob = nullptr;
            bool success = queue[oldValue & (queueLength - 1)].compare_exchange_strong(expectedJob, job);

            // Regardless of who wrote the slot, we will update the tail (if the successful thread got scheduled out
            // after writing the pointer we still want to be able to continue)
            tail.compare_exchange_strong(oldValue, oldValue + 1);

            // If we successfully added our job we're done
            if (success)
                break;
        }

        // Wake up a thread to process the job
        semaphore.Release();
    }
}

// Class Job
namespace Deep {
    JobSystem::Job::Job(JobSystem* jobSystem, JobFunction jobFunction, uint32 numDependencies) :
        jobSystem(jobSystem), jobFunction(jobFunction), numDependencies(numDependencies) {

    }
}