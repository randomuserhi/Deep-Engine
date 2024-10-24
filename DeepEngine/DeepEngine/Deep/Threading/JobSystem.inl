#pragma once

#include "./JobSystem.h" // TODO(randomuserhi): Remove 

// Class JobHandle
namespace Deep {
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
}
