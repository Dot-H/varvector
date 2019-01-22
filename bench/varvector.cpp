#include <vector>
#include <variant>
#include <string>
#include <random>

#include <stdlib.h>

#include <benchmark/benchmark.h>
#include "varvector.h"

template<class... Ts>
struct overloaded : Ts... { 
    using Ts::operator()...;
};

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

struct IShape {
    struct Coo {
        std::size_t x;
        std::size_t y;
    };

    IShape() : center{} {}
    IShape(const Coo& c) : center(c) {}
    virtual ~IShape() {}

    virtual std::size_t perimeter() const = 0;

    Coo center;
};

struct ASquare : public IShape {
    ASquare(std::size_t s)
        : IShape(), side(s) {}
    ASquare(const Coo& c, std::size_t s)
        : IShape(c), side(s) {}
    virtual ~ASquare() {}

    std::size_t perimeter() const override {
        std::size_t p = side * 4;
        benchmark::DoNotOptimize(p);
        return p;
    }

    std::size_t side = 0;
};

struct ATriangle : public IShape {
    ATriangle()
        : IShape() {}
    ATriangle(std::size_t p1,std::size_t p2,std::size_t p3)
        : IShape(), a(p1), b(p2), c(p3) {}
    ATriangle(const Coo& c, std::size_t p1,std::size_t p2,std::size_t p3)
        : IShape(c), a(p1), b(p2), c(p3) {}
    virtual ~ATriangle() {}

    std::size_t perimeter() const override {
        std::size_t p = a + b + c;
        benchmark::DoNotOptimize(p);
        return p;
    }

    std::size_t a = 0;
    std::size_t b = 0;
    std::size_t c = 0;
};

template <class D>
struct CRTPShape {
    struct Coo {
        std::size_t x;
        std::size_t y;
    };

    CRTPShape() : center{} {}
    CRTPShape(const Coo& c) : center(c) {}

    std::size_t perimeter() const {
        return static_cast<D*>(this)->perimeter();
    }

    Coo center;
};

struct CRTPSquare : public CRTPShape<CRTPSquare> {
    CRTPSquare(std::size_t s)
        : CRTPShape(), side(s) {}
    CRTPSquare(const Coo& c, std::size_t s)
        : CRTPShape(c), side(s) {}

    std::size_t perimeter() const {
        std::size_t p = side * 4;
        benchmark::DoNotOptimize(p);
        return p;
    }

    std::size_t side = 0;
};

struct CRTPTriangle : public CRTPShape<CRTPTriangle> {
    CRTPTriangle()
        : CRTPShape() {}
    CRTPTriangle(std::size_t p1,std::size_t p2,std::size_t p3)
        : CRTPShape(), a(p1), b(p2), c(p3) {}
    CRTPTriangle(const Coo& c, std::size_t p1,std::size_t p2,std::size_t p3)
        : CRTPShape(c), a(p1), b(p2), c(p3) {}

    std::size_t perimeter() const {
        std::size_t p = a + b + c;
        benchmark::DoNotOptimize(p);
        return p;
    }

    std::size_t a = 0;
    std::size_t b = 0;
    std::size_t c = 0;
};

template <class T>
static inline void init_vec(T& dst, const std::size_t n) {
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution uniform_dist(0, 1);
    for (std::size_t i = 0; i < n; ++i) {
        if (uniform_dist(e1) == 0) {
            dst.push_back(CRTPTriangle(i, i + 1, i +2));
        } else {
            dst.push_back(CRTPSquare(i));
        }
    }
}

template <>
inline void init_vec(std::vector<IShape*>& dst, const std::size_t n) {
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution uniform_dist(0, 1);
    for (std::size_t i = 0; i < n; ++i) {
        if (uniform_dist(e1) == 0) {
            dst.push_back(new ATriangle(i, i + 1, i +2));
        } else {
            dst.push_back(new ASquare(i));
        }
    }
}

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
BENCHMARK(BM_VirtuIterate)->Range(8, 8<<15);

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

BENCHMARK(BM_VariantIterate)->Range(8, 8<<15);

static void BM_VarvectorIterate(benchmark::State& state) {
  varvector<CRTPTriangle, CRTPSquare> v;
  init_vec(v, state.range(0));
  for (auto _ : state) {
      v.foreach([](auto&& el) {
        el.perimeter();
      });
  }
}

BENCHMARK(BM_VarvectorIterate)->Range(8, 8<<15);

static void BM_StableVarvectorIterate(benchmark::State& state) {
  stable_varvector<CRTPTriangle, CRTPSquare> v;
  init_vec(v, state.range(0));
  for (auto _ : state) {
      v.foreach([](auto&& el) {
        el.perimeter();
      });
  }
}

BENCHMARK(BM_StableVarvectorIterate)->Range(8, 8<<15);

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