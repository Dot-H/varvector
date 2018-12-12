#include <vector>
#include <variant>
#include <string>
#include <iostream>

#include <stdlib.h>

#include <benchmark/benchmark.h>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

struct IShape {
    struct Coo {
        std::size_t x;
        std::size_t y;
    };

    IShape() : center{} {}
    IShape(const Coo& c) : center(c) {}

    virtual std::size_t perimeter() const = 0;

    Coo center;
};

struct ASquare : public IShape {
    ASquare(std::size_t s)
        : IShape(), side(s) {}
    ASquare(const Coo& c, std::size_t s)
        : IShape(c), side(s) {}

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

struct CRTPSquare : public CRTPShape<ASquare> {
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

struct CRTPTriangle : public CRTPShape<ATriangle> {
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


/*********************************************
 *          varvector implementation         *
 *********************************************/

namespace details {

template <int N, typename Srch, typename Fst, typename... Ts>
constexpr int find_impl() {
    if constexpr (std::is_same_v<Srch, Fst>) {
        return N;
    } else if constexpr (sizeof...(Ts) == 0) {
        return -1;
    } else {
        return find_impl<N+1, Srch, Ts...>();
    }
}

template <typename Srch, typename... Ts>
constexpr int find_T() {
    return find_impl<0, Srch, Ts...>();
}

template <std::size_t N, typename Tuple, typename Func>
constexpr void foreachcont_impl(Tuple& tpl, Func fn) {
    if constexpr (N < std::tuple_size_v<Tuple>) {
        const auto& container = std::get<N>(tpl);
        for (auto&& el : container) {
            fn(el);
        }
        foreachcont_impl<N+1>(tpl, fn);
    }
}

template <std::size_t N, typename Tuple, typename Func>
constexpr void foreach_impl(Tuple& tpl, Func fn) {
    if constexpr (N < std::tuple_size_v<Tuple>) {
        fn(std::get<N>(tpl));
        foreach_impl<N+1>(tpl, fn);
    }
}

template <std::size_t N, typename Tuple, typename Func>
constexpr void switch_i(Tuple& tpl, std::size_t n, std::size_t i, Func fn) {
    if constexpr (N < std::tuple_size_v<Tuple>) {
        if (n == N) {
            fn(std::get<N>(tpl)[i]);
        }

        switch_i<N+1>(tpl, n, i, fn);
    }
}


} // namespace details

template <typename... Ts>
class varvector {
public:
    template <typename T>
    constexpr void push_back(T&& value) {
        std::get<details::find_T<T, Ts...>()>(vecs).push_back(std::forward<T>(value));
    }

    template <typename Func>
    constexpr void foreach(Func fn) {
        details::foreachcont_impl<0>(vecs, fn);
    }

    template <typename Func>
    constexpr void foreach(Func fn) const {
        details::foreachcont_impl<0>(vecs, fn);
    }

    constexpr std::size_t size() const {
        std::size_t n = 0;
        details::foreach_impl<0>(vecs, [&n](auto&& v) {
            n += v.size();
        });

        return n;
    }

    std::tuple<std::vector<Ts>...> vecs;
};

template <typename... Ts>
struct stable_varvector : public varvector<Ts...> {
public:
    template <typename T>
    constexpr void push_back(T&& value) {
        constexpr std::size_t val = details::find_T<T, Ts...>();
        static_assert(val <= uint8_t(-1), "The vector cannot contain more than 255 types");

        insertionsOrder.push_back(val);
        std::get<val>(varvector<Ts...>::vecs).push_back(std::forward<T>(value));
    }

    template <typename Func>
    void foreach(Func fn) {
        std::size_t curIdx[sizeof...(Ts)] = { 0 };

        for (const std::size_t i_vec : insertionsOrder) {
            details::switch_i<0>(
                    varvector<Ts...>::vecs,
                    i_vec,
                    curIdx[i_vec],
                    fn);
            curIdx[i_vec]++;
        }
    }

    void reserve(std::size_t n) {
        insertionsOrder.reserve(n);
    }

private:
    std::vector<uint8_t> insertionsOrder;
};

static inline void init_vec(std::vector<IShape*>& dst, const std::size_t n) {
  srand(42);
  for (std::size_t i = 0; i < n; ++i) {
      if (rand() % 2) {
          dst.push_back(new ATriangle(i, i + 1, i +2));
      } else {
          dst.push_back(new ASquare(i));
      }
  }
}

static inline void init_vec(std::vector<std::variant<CRTPTriangle, CRTPSquare>>& dst, const std::size_t n) {
  srand(42);
  for (std::size_t i = 0; i < n; ++i) {
      if (rand() % 2) {
          dst.emplace_back(CRTPTriangle(i, i + 1, i +2));
      } else {
          dst.emplace_back(CRTPSquare(i));
      }
  }
}

static inline void init_vec(varvector<CRTPTriangle, CRTPSquare>& dst, const std::size_t n) {
  srand(42);
  for (std::size_t i = 0; i < n; ++i) {
      if (rand() % 2) {
          dst.push_back(CRTPTriangle(i, i + 1, i +2));
      } else {
          dst.push_back(CRTPSquare(i));
      }
  }
}

static inline void init_vec(stable_varvector<CRTPTriangle, CRTPSquare>& dst, const std::size_t n) {
  srand(42);
  for (std::size_t i = 0; i < n; ++i) {
      if (rand() % 2) {
          dst.push_back(CRTPTriangle(i, i + 1, i +2));
      } else {
          dst.push_back(CRTPSquare(i));
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
}
//
// Register the function as a benchmark
//BENCHMARK(BM_VirtuIterate)->Range(8, 8<<15);

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

//BENCHMARK(BM_VariantIterate)->Range(8, 8<<15);

static void BM_VarvectorIterate(benchmark::State& state) {
  varvector<CRTPTriangle, CRTPSquare> v;
  init_vec(v, state.range(0));
  for (auto _ : state) {
      v.foreach([](auto&& el) {
        el.perimeter();
      });
  }
}

//BENCHMARK(BM_VarvectorIterate)->Range(8, 8<<15);

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

BENCHMARK_MAIN();
