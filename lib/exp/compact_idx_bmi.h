#include <vector>

#include <immintrin.h>

#define NBBITS_UINT 64

constexpr uint8_t cnt_nbits(uint8_t n) {
    if (n == 1) {
        return 1;
    }

    return (sizeof(uint32_t) * 8) - __builtin_clz(n - 1);
}

template <std::uint8_t MAX,
          uint8_t NBBITS_MAX = cnt_nbits(MAX)>
struct compact_idx_bmi {
    std::uint64_t* data() const { return pool.data(); }

    void push_back(uint8_t val) {
        const std::size_t idx = bit_off / NBBITS_UINT;
        std::uint64_t* bitset = pool.data() + idx;
        *bitset |= static_cast<std::uint64_t>(val) << (bit_off % NBBITS_UINT);

        bit_off += NBBITS_MAX;
        if ((NBBITS_UINT - (bit_off % NBBITS_UINT)) <= NBBITS_MAX) {
            pool.push_back(0);
        }
    }

    std::vector<std::uint64_t> pool = { 0 };
    std::size_t bit_off = 0;
};
