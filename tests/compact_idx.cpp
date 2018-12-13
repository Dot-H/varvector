#include <iostream>
#include <vector>

constexpr uint16_t cnt_nbits(std::size_t n) {
    if (n == 1) {
        return 1;
    }

    return (sizeof(std::size_t) * 8) - __builtin_clzll(n - 1);
}

template <std::uint8_t MAX,
          uint8_t nbits = cnt_nbits(MAX)>
struct compact_idx {
    struct iterator {
        iterator& operator++() { 
            idx += nbits;
            return *this;
        }

        uint8_t operator*() const {
             return *reinterpret_cast<const uint8_t*>(data + idx) & ((1ull << nbits) - 1);
        }

        bool operator!= (const iterator& o) const {
            return (data+idx) == (o.data+idx);
        }

        std::size_t idx;
        const uint64_t* data;
    };

    iterator begin() const { return iterator{0, pool.data()}; }
    iterator end()   const { return iterator{pool.size(), pool.data()}; }

    void push_back(uint8_t val) {
        *reinterpret_cast<uint8_t*>(pool.data() + idx) = val;
        idx += nbits;
        if ((8 - (idx % 8)) <= nbits ) {
            pool.push_back(0);
        }
    }

    std::vector<uint64_t> pool = { 0 };
    std::size_t idx = 0;
};

int main() {
    compact_idx<3> idx;
    idx.push_back(1);
    idx.push_back(2);
    idx.push_back(0);
    idx.push_back(1);
    idx.push_back(3);
    idx.push_back(2);

    for (uint8_t t : idx) {
        std::cout << t << std::endl;
    }
}