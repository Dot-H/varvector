#include <iostream>

#include "exp/compact_idxv2.h"

int main() {
    compact_idx<254, uint8_t> idx;
    idx.push_back(1);
    idx.push_back(2);
    idx.push_back(0);
    idx.push_back(1);

    idx.push_back(2);
    idx.push_back(2);
    idx.push_back(0);
    idx.push_back(1);

    idx.push_back(2);
    idx.push_back(253);
    idx.push_back(0);
    idx.push_back(1);

    idx.push_back(2);
    idx.push_back(142);
    idx.push_back(0);
    idx.push_back(1);

    idx.push_back(2);
    idx.push_back(232);
    idx.push_back(0);
    idx.push_back(1);

    for (uint64_t t : idx) {
        std::cout << t << std::endl;
    }
}