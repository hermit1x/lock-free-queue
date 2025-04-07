#include <atomic>
#include <thread>
#include "benchmark/benchmark.h"
#include "queues/std_queue_with_mutex.hpp"

using TestQueue = std_queue_with_mutex<int>;

template <typename T>
static void BM_QueueTest(benchmark::State& state) {
    T queue;
    const int ops_per_thread = 1e5;

    auto worker = [&](bool is_producer) {
        if (is_producer) {
            for (int i = 0; i < ops_per_thread; ++i) {
                queue.push(i);
            }
        }
        else {
            int count = 0;
            int val;
            while (count < ops_per_thread) {
                if (queue.try_pop(val)) {
                    ++count;
                    benchmark::DoNotOptimize(val);
                }
            }
        }
    };

    for (auto _ : state) {
        std::vector<std::thread> threads;
        for (int i = 0; i < state.threads(); ++i) {
            threads.emplace_back(worker, i % 2 == 0);
        }
        for (auto &t : threads) t.join();
    }
    state.SetItemsProcessed(state.iterations() * ops_per_thread * state.threads());
}
BENCHMARK_TEMPLATE(BM_QueueTest, std_queue_with_mutex<int>)
    ->Name("std_queue_mutex")
    ->ThreadRange(2, 64)
    ->UseRealTime();

BENCHMARK_MAIN();