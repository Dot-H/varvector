#include <iterator>
#include <vector>
#include <numeric>

constexpr uint8_t cnt_nbits(uint8_t n) {
    if (n == 1) {
        return 1;
    }

    return (sizeof(uint32_t) * 8) - __builtin_clz(n - 1);
}

template <std::uint8_t MAX,
          typename UINT = uint64_t,
          uint8_t NBBITS_MAX = cnt_nbits(MAX)>
struct compact_idx {
    struct Iterator {
        Iterator& operator++() { 
            idx += NBBITS_MAX;
            return *this;
        }

        uint8_t operator*() const {
            uint8_t acc = 0;
            for (int i = NBBITS_MAX - 1; i >= 0; --i) {
                acc = (acc << 1) + data[idx + i];
            }

            return acc;
        }

        bool operator!= (const Iterator& o) const {
            return idx != o.idx;
        }

        std::size_t idx;
        const std::vector<bool>& data;
    };

    Iterator begin() const { return Iterator{0, pool}; }
    Iterator end()   const { return Iterator{pool.size(), pool}; }

    void push_back(uint8_t val) {
        for (std::size_t i = 0; i < NBBITS_MAX; ++i, val >>= 1) {
            pool.push_back(val & 1);
        }
    }

    std::vector<bool> pool;
};
