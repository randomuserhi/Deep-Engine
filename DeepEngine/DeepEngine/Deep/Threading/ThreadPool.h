#pragma once

#include "../../Deep.h"

#include "../NonCopyable.h"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

// TODO(randomuserhi): Refactor => needs to support proper task distribution (ensure all cores are utilized fully)
// TODO(randomuserhi): Thread affinity ?? => seems platform dependent

//#include <windows.h>

namespace Deep {
    class ThreadPool : NonCopyable {
    private:
        void Worker();

        std::mutex mutex;
        std::condition_variable cvJob;

        std::condition_variable cvCompleted;
        size_t busy;
        bool running;

        std::queue<std::function<void()>> jobs;
        std::vector<std::thread> pool;

    public:
        ThreadPool(size_t numThreads) : running(true), numThreads(numThreads), busy(0) {
            for (size_t i = 0; i < numThreads; ++i) {
                pool.emplace_back(std::thread(&ThreadPool::Worker, this));
                //DWORD_PTR dw = SetThreadAffinityMask(pool.back().native_handle(), DWORD_PTR(1) << i);
            }
        }
        ~ThreadPool() {
            {
                std::lock_guard<std::mutex> lock(mutex);
                running = false;
            }
            cvJob.notify_all();

            for (size_t i = 0; i < pool.size(); ++i) {
                pool[i].join();
            }
        }

        void Enqueue(const std::function<void()>& job);

        void Join();

        const int32 numThreads;
    };
}