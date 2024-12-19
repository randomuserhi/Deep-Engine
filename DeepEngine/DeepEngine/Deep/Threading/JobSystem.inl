#pragma once

#include "./JobSystem.h" // TODO(randomuserhi): Remove

// Class JobSystem
namespace Deep {
    void JobSystem::FreeJob(JobSystem::Job* job) {
        jobs.FreeItem(job);
    }

    void JobSystem::ReleaseBarrier(JobSystem::Barrier* barrier) {
        barriers.ReleaseItem(barrier);
    }
} // namespace Deep

// Class Job
namespace Deep {
    void JobSystem::Job::Acquire() {
        referenceCount.fetch_add(1, std::memory_order_relaxed);
    }

    void JobSystem::Job::Release() {
        // TODO(randomuserhi): Learn why the release semantics are this way...

        // Releasing a reference must use release semantics...
        if (referenceCount.fetch_sub(1, std::memory_order_release) == 1) {
            // ... so that we can use acquire to ensure that we see any updates from other threads that released a ref before
            // freeing the job
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

    void JobSystem::Job::SetDependencies(uint32 count) {
        // TODO(randomuserhi): Checks for if dependencies are set while a job is already running or is finished etc...
        numDependencies.store(count, std::memory_order_relaxed);
    }

    void JobSystem::Job::AddDependency(uint32 count) {
        // TODO(randomuserhi): Checks for if dependency is added while a job is already running or is finished etc...
        numDependencies.fetch_add(count, std::memory_order_relaxed);
    }

    void JobSystem::Job::RemoveDependency(uint32 count) {
        uint32 oldCount = numDependencies.fetch_sub(count, std::memory_order_release);
        // TODO(randomuserhi): Checks for if dependency is added while a job is already running or is finished etc...
        uint32 newCount = oldCount - count;
        Deep_Assert(oldCount > newCount, "Logical Error. Removed more dependencies than there were available.");

        // Queue the job if dependencies == 0
        if (newCount == 0) {
            jobSystem->QueueJobInternal(this);
        }
    }

    void JobSystem::Job::Execute() {
        // TODO(randomuserhi): Check executing state (dont run job if it already executed / executing) -> assert only?
        //                     Check number of dependencies == 0 -> assert only?

        jobFunction();
    }
} // namespace Deep

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

    JobSystem::JobHandle::~JobHandle() {
        Release();
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

    JobSystem::JobHandle& JobSystem::JobHandle::operator=(JobSystem::Job* job) {
        if (this->job != job) {
            Release();
            this->job = job;
            Acquire();
        }
        return *this;
    }

    JobSystem::JobHandle& JobSystem::JobHandle::operator=(const JobSystem::JobHandle& handle) {
        if (job != handle.job) {
            Release();
            job = handle.job;
            Acquire();
        }
        return *this;
    }

    JobSystem::JobHandle& JobSystem::JobHandle::operator=(JobSystem::JobHandle&& handle) noexcept {
        if (job != handle.job) {
            Release();
            job = handle.job;
            handle.job = nullptr;
        }
        return *this;
    }

    JobSystem::JobHandle::operator JobSystem::Job*() const {
        return job;
    }

    JobSystem::Job* JobSystem::JobHandle::operator->() const {
        Deep_Assert(job != nullptr, "JobHandle is nullptr.");
        return job;
    }

    JobSystem::Job& JobSystem::JobHandle::operator*() const {
        Deep_Assert(job != nullptr, "JobHandle is nullptr.");
        return *job;
    }

    bool JobSystem::JobHandle::operator==(const Job* b) const {
        return job == b;
    }
    Deep_Inline bool operator==(JobSystem::JobHandle& a, JobSystem::JobHandle& b) {
        return a.job == b.job;
    }
    bool JobSystem::JobHandle::operator!=(const JobSystem::Job* b) const {
        return !(*this == b);
    }
    Deep_Inline bool operator!=(JobSystem::JobHandle& a, JobSystem::JobHandle& b) {
        return !(a == b);
    }

    JobSystem::Job* JobSystem::JobHandle::GetPtr() const {
        return job;
    }

    void JobSystem::JobHandle::AddDependency(uint32 count = 1) const {
        Deep_Assert(job != nullptr, "JobHandle is nullptr.");
        job->AddDependency(count);
    }

    void JobSystem::JobHandle::RemoveDependency(uint32 count = 1) const {
        Deep_Assert(job != nullptr, "JobHandle is nullptr.");
        job->RemoveDependency(count);
    }
} // namespace Deep

// Class Barrier
namespace Deep {
    bool JobSystem::Barrier::IsEmpty() {
        return readIndex == writeIndex;
    }
} // namespace Deep
