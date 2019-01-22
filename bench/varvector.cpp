#include <vector>
#include <variant>
#include <string>
#include <random>

#include <stdlib.h>

#include <benchmark/benchmark.h>
#include "varvector.h"
#include "utils.h"
#include "exp/varvector_cpct.h"

static void BM_VirtuIterate(benchmark::State& state) {
  std::vector<IShape*> v;
  init_vec(v, state.range(0));
  for (auto _ : state) {
      for (const auto* el : v) {
          benchmark::DoNotOptimize(el->perimeter());
      }
  }

  for (auto* el : v) {
      delete el;
  }
}
BENCHMARK(BM_VirtuIterate)->RangeMultiplier(2)->Range(8, 8<<15);

static void BM_VariantIterate(benchmark::State& state) {
  std::vector<std::variant<CRTPTriangle, CRTPSquare>> v;
  init_vec(v, state.range(0));
  for (auto _ : state) {
      for (const auto& el : v) {
          std::visit(overloaded {
            [](CRTPTriangle arg) { arg.perimeter(); },
            [](CRTPSquare arg) { arg.perimeter(); }
        }, el);
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
