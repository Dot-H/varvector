#include <vector>

constexpr uint8_t cnt_nbits(uint8_t n) {
    if (n == 1) {
        return 1;
    }

    return (sizeof(uint32_t) * 8) - __builtin_clz(n - 1);
}

template <std::uint8_t MAX,
          typename UINT = uint64_t,
          uint8_t NBBITS_MAX = cnt_nbits(MAX),
          uint8_t NBBITS_UINT = sizeof(UINT) * 8>
struct compact_idx {
    struct iterator {
        iterator& operator++() { 
            bit_off += NBBITS_MAX;
            return *this;
        }

        uint8_t operator*() const {
            static thread_local const uint8_t MASK = ((1ull << NBBITS_MAX) - 1);

            const std::size_t idx = bit_off / NBBITS_UINT;
            const UINT bitset = *(data + idx);
            auto val = bitset >> (bit_off % NBBITS_UINT);

            return val & MASK;
        }

        bool operator!= (const iterator& o) const {
            return data != o.data || bit_off != o.bit_off;
        }

        std::size_t bit_off;
        const UINT* data;
    };

    iterator begin() const { return iterator{0, pool.data()}; }
    iterator end()   const { return iterator{bit_off, pool.data()}; }

    std::size_t size() const { return bit_off / NBBITS_MAX; }
    std::size_t pool_size() const { return pool.size(); }
    constexpr std::size_t nbbits_uint() const { return NBBITS_UINT; }
    constexpr std::size_t nbbits_max() const { return NBBITS_MAX; }
    const UINT* data() const { return pool.data(); }

    void push_back(uint8_t val) {
        const std::size_t idx = bit_off / NBBITS_UINT;
        UINT* bitset = reinterpret_cast<UINT*>(pool.data() + idx);
        *bitset |= static_cast<UINT>(val) << (bit_off % NBBITS_UINT);

        bit_off += NBBITS_MAX;
        if ((NBBITS_UINT - (bit_off % NBBITS_UINT)) <= NBBITS_MAX) {
            pool.push_back(0);
        }
    }

    std::vector<UINT> pool = { 0 };
    std::size_t bit_off = 0;
};
