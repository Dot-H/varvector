#include <string>
#include <random>

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
