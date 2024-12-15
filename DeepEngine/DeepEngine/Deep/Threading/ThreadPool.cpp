/**
 * Thread Pool
 */

#include "./ThreadPool.h"

namespace Deep {
    void ThreadPool::Worker() {
        while (running) {
            std::unique_lock<std::mutex> lock(mutex);
            cvJob.wait(lock, [this] { return !jobs.empty() || !running; });

            if (!jobs.empty()) {
                ++busy;

                std::function<void()> job = jobs.front();
                jobs.pop();

                lock.unlock();

                job();

                lock.lock();
                --busy;
                cvCompleted.notify_all();
            } else if (!running) {
                break;
            }
        }
    }

    void ThreadPool::Enqueue(const std::function<void()>& job) {
        std::unique_lock<std::mutex> lock(mutex);
        jobs.push(job);
        cvJob.notify_one();
    }

    void ThreadPool::Join() {
        std::unique_lock<std::mutex> lock(mutex);
        cvCompleted.wait(lock, [this] { return jobs.empty() && busy == 0; });
    }
} // namespace Deep