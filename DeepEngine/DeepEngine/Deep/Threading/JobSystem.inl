#pragma once

#include "./JobSystem.h" // TODO(randomuserhi): Remove 

// Class JobSystem
namespace Deep {
    void JobSystem::FreeJob(JobSystem::Job* job) {
        jobs.FreeItem(job);
    }
}

// Class Job
namespace Deep {
    void JobSystem::Job::Acquire() {
        referenceCount.fetch_add(1, std::memory_order_relaxed);
    }

    void JobSystem::Job::Release() {
        // TODO(randomuserhi): Learn why the release semantics are this way...

        // Releasing a reference must use release semantics...
        if (referenceCount.fetch_sub(1, std::memory_order_release) == 1) {
            // ... so that we can use acquire to ensure that we see any updates from other threads that released a ref before freeing the job
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

    void JobSystem::Job::AddDependency(uint32 count) {
        // TODO(randomuserhi): Checks for if dependency is added while a job is already running or is finished etc...
        numDependencies.fetch_add(count, std::memory_order_relaxed);
    }

    void JobSystem::Job::RemoveDependency(uint32 count) {
        uint32 oldCount = numDependencies.fetch_sub(count, std::memory_order_release);
        // TODO(randomuserhi): Checks for if dependency is added while a job is already running or is finished etc...
        uint32 newCount = oldCount - count;
        Deep_Assert(oldCount > newCount, "Test wrap around, this is a logic error (Removed more dependencies than there are).");

        // Queue the job if dependencies == 0
        if (newCount == 0) {
            jobSystem->Enqueue(this);
        }
    }

    void JobSystem::Job::Execute() {
        // TODO(randomuserhi): Check executing state (dont run job if it already executed / executing) -> assert only?
        //                      - Maybe don't check if executed to allow jobs to be re-run (reused)
        //                     Check number of dependencies == 0 -> assert only?

        jobFunction();
    }
}

// Class JobHandle
namespace Deep {
    JobSystem::JobHandle::JobHandle(JobSystem::Job* job) :
        job(job) {
        Acquire();
    }

    JobSystem::JobHandle::JobHandle(const JobSystem::JobHandle& handle) :
        job(handle.job) {
        Acquire();
    }

    JobSystem::JobHandle::JobHandle(JobSystem::JobHandle&& handle) noexcept :
        job(handle.job) {
        handle.job = nullptr;
    }

    void JobSystem::JobHandle::Acquire() {
        if (job != nullptr) {
            job->Acquire();
        }
    }

    void JobSystem::JobHandle::Release() {
        if (job != nullptr) {
            job->Release();
        }
    }

    JobSystem::JobHandle& JobSystem::JobHandle::operator= (JobSystem::Job* job) {
        Release();
        this->job = job;
        Acquire();
    }

    JobSystem::JobHandle& JobSystem::JobHandle::operator= (const JobSystem::JobHandle& handle) {
        Release();
        job = handle.job;
        Acquire();
    }

    JobSystem::JobHandle& JobSystem::JobHandle::operator= (JobSystem::JobHandle&& handle) noexcept {
        Release();
        job = handle.job;
        handle.job = nullptr;
    }

    JobSystem::JobHandle::operator JobSystem::Job* () const {
        return job;
    }

    JobSystem::Job* JobSystem::JobHandle::operator-> () const {
        return job;
    }

    JobSystem::Job& JobSystem::JobHandle::operator* () const {
        return *job;
    }

    bool JobSystem::JobHandle::operator== (const JobSystem::Job* b) const {
        return job == b;
    }
    bool operator== (JobSystem::JobHandle& a, JobSystem::JobHandle& b) {
        return a.job == b.job;
    }
    bool JobSystem::JobHandle::operator!= (const JobSystem::Job* b) const {
        return !(*this == b);
    }
    bool operator!= (JobSystem::JobHandle& a, JobSystem::JobHandle& b) {
        return !(a == b);
    }

    JobSystem::Job* JobSystem::JobHandle::GetPtr() const {
        return job;
    }

    void JobSystem::JobHandle::AddDependency(uint32 count = 1) const {
        job->AddDependency(count);
    }

    void JobSystem::JobHandle::RemoveDependency(uint32 count = 1) const {
        job->RemoveDependency(count);
    }
}
