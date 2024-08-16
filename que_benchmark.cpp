#include <queue>

#include "benchmark/benchmark.h"

static void BM_Nothing(benchmark::State& state) {
    for (auto _ : state) {
        ;
    }
}
BENCHMARK(BM_Nothing);

BENCHMARK_MAIN();