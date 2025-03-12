#define DEEP_ENABLE_ASSERTS

#include <iostream>

#include <Deep.h>
#include <Deep/Net.h>
#include <Deep/Math.h>
#include <Deep/Threading.h>

// https://stackoverflow.com/a/62047818/9642458
#include <chrono>
#include <thread>

#define THREADED

int main() {
    Deep::JobSystem jobSystem{ static_cast<int>(8 - 1), 2048, 1024 };

    constexpr size_t count = 1000000000;
    constexpr size_t slice = 500000;
    static_assert(count % slice == 0, "");

    Deep::Vec3* positions = new Deep::Vec3[count];
    Deep::Vec3* velocities = new Deep::Vec3[count];

    for (size_t i = 0; i < count; ++i) {
        positions[i] = Deep::Vec3::zero;
        velocities[i] = Deep::Vec3::left;
    }

#ifdef THREADED
    Deep::Barrier barrier = jobSystem.AcquireBarrier();
#endif

    auto start = std::chrono::system_clock::now();

#ifdef THREADED
    for (size_t i = 0; i < count / slice; ++i) {
        barrier.AddJob(jobSystem.Enqueue([slice, i, &positions, &velocities]() {
            for (size_t j = i * slice; j < i * slice + slice; ++j) {
                positions[j] += velocities[j];
            }
        }));
    }

    barrier.Wait();
#else
    {
        size_t i;
#pragma omp parallel for
        for (i = 0; i < count; ++i) {
            positions[i] += velocities[i];
        }
    }
#endif

    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << elapsed.count() << "ms\n";

    for (size_t i = 0; i < 5; ++i) {
        std::cout << positions[i].x << "\n";
    }
    std::cout << "...\n";
    for (size_t i = count - 6; i < count; ++i) {
        std::cout << positions[i].x << "\n";
    }
}