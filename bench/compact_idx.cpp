
#include <benchmark/benchmark.h>

#include "utils.h"
#include "compact_idx_bmi.h"
#include "compact_idx.h"

static void BM_compact_idx(benchmark::State& state) {
    compact_idx<3> idxs;
    init_idx(idxs, state.range(0), 0, 2);
    for (auto _ : state) {
        for (std::size_t i = 0; i < idxs.size()) {

        }
    }
}

BENCHMARK(BM_VariantIterate)->RangeMultiplier(2)->Range(8, 8<<15);

static void BM_VarvectorIterate(benchmark::State& state) {
  varvector<CRTPTriangle, CRTPSquare> v;
  init_vec(v, state.range(0));
  for (auto _ : state) {
      v.foreach([](auto&& el) {
        el.perimeter();
      });
  }
}

BENCHMARK(BM_VarvectorIterate)->RangeMultiplier(2)->Range(8, 8<<15);

static void BM_StableVarvectorIterate(benchmark::State& state) {
  stable_varvector<CRTPTriangle, CRTPSquare> v;
  init_vec(v, state.range(0));
  for (auto _ : state) {
      v.foreach([](auto&& el) {
        el.perimeter();
      });
  }
}

BENCHMARK(BM_StableVarvectorIterate)->RangeMultiplier(2)->Range(8, 8<<15);

static void BM_VariantPushBack(benchmark::State& state) {
    std::vector<std::variant<CRTPTriangle, CRTPSquare>> v;
    for (auto _ : state) {
        init_vec(v, state.range(0));
    }
}

BENCHMARK(BM_VariantPushBack)->Range(8, 8<<15);

static void BM_VarvectorPushBack(benchmark::State& state) {
    varvector<CRTPTriangle, CRTPSquare> v;
    for (auto _ : state) {
        init_vec(v, state.range(0));
    }
}

BENCHMARK(BM_VarvectorPushBack)->Range(8, 8<<15);

static void BM_StableVarvectorPushBack(benchmark::State& state) {
    stable_varvector<CRTPTriangle, CRTPSquare> v;
    for (auto _ : state) {
        init_vec(v, state.range(0));
    }
}

BENCHMARK(BM_StableVarvectorPushBack)->Range(8, 8<<15);

BENCHMARK_MAIN();
