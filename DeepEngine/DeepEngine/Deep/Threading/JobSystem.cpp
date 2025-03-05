/**
 * Job System
 */

#include <Deep/Threading/JobSystem.h>

// Class JobSystem
namespace Deep {
    JobSystem::JobSystem(int32 numThreads, uint32 maxJobs, uint32 maxBarriers) :
        jobs(maxJobs, maxJobs), numThreads(numThreads), barriers(maxBarriers, maxBarriers) {
        Deep_Assert(numThreads > 0, "Number of threads must be greater than 0.");
        Deep_Assert(maxBarriers > 0, "Maximum number of barriers must be greater than 0.");

        StartThreads();
    }

    JobSystem::~JobSystem() {
        StopThreads();
    }

    void JobSystem::StartThreads() {
        if (threads != nullptr || numThreads == 0) return;

        running = true;

        // Allocate heads for job queue per thread
        heads = reinterpret_cast<std::atomic<uint32>*>(Malloc(sizeof(std::atomic<uint32>) * numThreads));
        for (int32 i = 0; i < numThreads; ++i) {
            heads[i] = 0;
        }

        // Allocate and start threads
        threads = reinterpret_cast<std::thread*>(Malloc(numThreads * sizeof(std::thread)));
        for (int32 i = 0; i < numThreads; ++i) {
            ::new (threads + i) std::thread([this, i] {
                ThreadMain(i);
            });

            // TODO(randomuserhi): Someway to manage thread affinity ?
            // NOTE(randomuserhi): Account for hyperthreading
            //                     - `i*2` here to account for hyperthreading so 2 threads aren't put on the same physical
            //                       core.
            // DWORD_PTR dw = ::SetThreadAffinityMask(threads[i].native_handle(), DWORD_PTR(1) << (i * 2));
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
        std::atomic<uint32>& head = heads[id];

        while (running) {
            semaphore.Acquire();

            while (head != tail) {
                std::atomic<JobSystem::Job*>& job = queue[head & (queueLength - 1)];
                if (job.load() != nullptr) {
                    JobSystem::Job* jobPtr = job.exchange(nullptr);
                    if (jobPtr != nullptr) {
                        // NOTE(randomuserhi): We do not need to check if the job has already been executed
                        //                     as `Execute()` will early return automatically if it has.
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
            index = jobs.AllocItem(this, jobFunction, numDependencies);
            if (index != FixedSizeFreeList<Job>::invalidItemIndex) {
                break;
            }

            // If we can't, then stall
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        Job* const job = &jobs[index];

        // Create a reference to the job
        // NOTE(randomuserhi): Has to be done prior queueing the job, as if the job completes
        //                     prior the creation of this reference, the job gets released.
        JobSystem::JobHandle ref{ job };

        // Queue the job if it has no dependencies
        if (numDependencies == 0) {
            QueueJobInternal(job);
        }

        // Return handle
        return ref;
    }

    uint32 JobSystem::GetMinHead() {
        uint32 head = tail;
        for (size_t i = 0; i < numThreads; ++i)
            head = Deep::Min(head, heads[i].load());

        return head;
    }

    void JobSystem::QueueJobInternal(JobSystem::Job* job) {
        // TODO(randomuserhi): Check job belongs to this job system -> assert only?
        //                     Check job has not already been executed -> assert only?
        //                     Check job is not already executing -> assert only?
        //                     Check job has 0 dependencies -> assert only?
        //                     Check we have worker threads and that the job system is running -> assert only?

        // Acquire a reference to the job to keep it alive in job queue
        job->Acquire();

        // Get the head that has processed the least amount of jobs
        uint32 head = GetMinHead();

        // Wake up a thread
        // NOTE(randomuserhi): We read the head outside of the loop since it involves iterating over all threads and we only
        // need to update
        //                     it if there's not enough space in the queue.
        semaphore.Release();

        for (;;) {
            // Check if there is space in the queue
            uint32 oldValue = tail;
            if (oldValue - head >= queueLength) {
                // We calculated the head outside of the loop, update head (and we also need to update tail to prevent it
                // from passing head)
                head = GetMinHead();
                oldValue = tail;

                // Second check if there's space in the queue
                if (oldValue - head >= queueLength) {
                    // Wake up all threads in order to ensure that they can clear any nullptrs they may not have processed
                    // yet
                    semaphore.Release(numThreads);

                    // Stall a little (we have to wait for other threads to update their head pointer in order for us to be
                    // able to continue)
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
            if (success) break;
        }

        // Wake up a thread to process the job
        semaphore.Release();
    }

    JobSystem::BarrierHandle JobSystem::AcquireBarrier() {
        // Loop until we can acquire a barrier in the free list
        uint32 index;
        for (;;) {
            index = barriers.AcquireItem();
            if (index != PooledFixedSizeFreeList<JobSystem::Barrier>::invalidItemIndex) {
                break;
            }

            // If we can't, then stall
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        Barrier* barrier = &barriers[index];
        barrier->jobSystem = this; // Set owning job system

        return BarrierHandle{ barrier };
    }
} // namespace Deep

// Class Job
namespace Deep {
    JobSystem::Job::Job(JobSystem* jobSystem, JobFunction jobFunction, uint32 numDependencies) :
        jobSystem(jobSystem), jobFunction(jobFunction), numDependencies(numDependencies) {}
} // namespace Deep

// Class Barrier
namespace Deep {
    JobSystem::Barrier::Barrier() :
        jobSystem(nullptr) {
        for (size_t i = 0; i < maxJobs; ++i) {
            jobs[i] = nullptr;
        }
    }

    JobSystem::Barrier::~Barrier() {
        Deep_Assert(IsEmpty(), "Barrier must be empty.");
    }

    void JobSystem::Barrier::AddJob(const JobHandle& job) {
        if (job.IsDone()) {
            // If the job has already finished executing, skip it
            return;
        }

        bool releaseSemaphore = false;

        Job* jobPtr = job.GetPtr();
        Deep_Assert(jobSystem == jobPtr->jobSystem, "Job and Barrier must belong to the same job system.");
        if (jobPtr->SetBarrier(this)) {
            numToAcquire++;
            if (jobPtr->CanBeExecuted()) {
                // If the job can be executed we want to release the semaphore to allow the waiting thread to
                // start executing it
                releaseSemaphore = true;
                numToAcquire++;
            }

            // Add the job to our job list
            jobPtr->Acquire();
            uint32 writeIndex = this->writeIndex++;
            while (writeIndex - readIndex >= maxJobs) {
                // Barrier is full, stall and wait for space
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
            jobs[writeIndex & (maxJobs - 1)] = jobPtr;
        }

        // Notify waiting thread that a new executable job is available
        // NOTE(randomuserhi): Should only be called if the passed job has 0 dependencies,
        //                     and is thus executable.
        if (releaseSemaphore) {
            semaphore.Release();
        }
    }

    void JobSystem::Barrier::Wait() {
        // TODO(randomuserhi): More optimal barrier job queue...

        while (numToAcquire > 0) {
            // Go through all jobs
            bool hasExecuted;
            do {
                hasExecuted = false;

                // Loop through the jobs and erase jobs from the beginning of the list that are done
                while (readIndex < writeIndex) {
                    std::atomic<Job*>& job = jobs[readIndex & (maxJobs - 1)];
                    Job* jobPtr = job.load();
                    Deep_Assert(jobPtr != nullptr, "Invalid job pointer was encountered.");
                    if (!jobPtr->IsDone()) break;

                    // Job is finished, release it
                    jobPtr->Release();
                    job = nullptr;
                    ++readIndex;
                }

                // Loop through the jobs and execute the first executable job
                for (uint32 index = readIndex; index < writeIndex; ++index) {
                    const std::atomic<Job*>& job = jobs[index & (maxJobs - 1)];
                    Job* jobPtr = job.load();
                    Deep_Assert(jobPtr != nullptr, "Invalid job pointer was encountered.");
                    if (jobPtr->CanBeExecuted()) {
                        // Only execute the job if it has not already been executed
                        // and mark the fact that we executed a job ourself.
                        //
                        // NOTE(randomuserhi): `jobPtr->Execute() == Job::doneState` is not sufficient as the job could have
                        //                     been executed by different thread.
                        jobPtr->Execute();
                        hasExecuted = true;

                        break;
                    }
                }

            } while (hasExecuted);

            // Wait for another thread to wake us when either there is more work to do or when all jobs have completed

            // When there have been multiple releases, we acquire them all at the same time to avoid needlessly spinning on
            // executing jobs
            int numToAcquire = Max(1, semaphore.GetValue());
            semaphore.Acquire(numToAcquire);
            this->numToAcquire -= numToAcquire;
        }

        // Release all jobs from list
        while (readIndex < writeIndex) {
            std::atomic<Job*>& job = jobs[readIndex & (maxJobs - 1)];
            Job* jobPtr = job.load();
            Deep_Assert(jobPtr != nullptr, "Invalid job pointer was encountered.");
            Deep_Assert(jobPtr->IsDone(), "Not all jobs were completed.");
            jobPtr->Release();
            job = nullptr;
            ++readIndex;
        }
    }
} // namespace Deep