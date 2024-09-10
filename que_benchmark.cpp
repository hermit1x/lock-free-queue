#include "benchmark/benchmark.h"
#include "queues/basic_queue.h"
#include "queues/std_queue_with_mutex.hpp"

const int total_operates = 1 << 10;

static void BM_QUEUES(benchmark::State& state, basic_queue<int> *queue) {
    for (auto _ : state) {
        if (state.thread_index() % 2 == 0) {
            queue->push(1);
        } else {
            queue->pop();
        }
    }
}
std_queue_with_mutex<int> q;
BENCHMARK_CAPTURE(BM_QUEUES, std_queue_mutex, static_cast<basic_queue<int>*>(&q))->ThreadRange(2, 512);

BENCHMARK_MAIN();