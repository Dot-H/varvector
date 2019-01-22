# varvector

This repository contains the implementation of `varvector` and `stable_varvector`
classes.

## Example of use

Take a look at the `bench/varvector.cpp` file.
```bash
mkdir build && cd build
cmake ..
make [bench | varvector_test | compact_idx_test]
```

## Why

It makes possible to have a vector containing multiple types in a **efficient**
and **type safe** way. Currently an other "classical" way would be to use a
`std::vector` containing `std::variant` objects.

I started to develop it because in a tree I needed a vector with different type
of nodes in it. First option would have been to makes every node type inheriting
from a common type. But dynamic dispatch is super costly.

So I used **CRTP** to still have the common interface without the dynamic
dispatch cost and put the result in a `std::vector` of `std::variant`. The problem
is `std::variant` must use`std::visit` for each iteration.

This is the solution to heradicate any need of dynamic dispatch.

## varvector vs stable_varvector

If you need to keep the informations about the insertion order or the index of an
element: use `stable_varvector`. Otherwise, use `varvector`.

The difference is simple but makes a huge difference: `stable_varvector` keeps an
insertion order vector.

## Perf

### Iterations:
* `std::vector<std::variant>` ~1.17x faster than `std::vector<virtualization>`
* `varvector` ~5.1x faster than `std::vector<virtualization>`
* `stable_varvector` ~1.8x faster than `std::vector<virtualization>`

### Insertions:
* `varvector` ~1.5x faster than `std::vector<std::variant>`
* `stable_varvector` ~1.4x faster than `std::vector<std::variant>`

### Space:

`std::variant` and `virtualization` both add 8 bytes per **element** while
`varvector` has a base cost of 24 bytes per **type**. Meaning that it starts
to take less space once there are (3 * (nbTypes - 1)) elements in the vector.

### performance output from the `bench` executable:


```
Running ./bench
Run on (8 X 2900 MHz CPU s)
CPU Caches:
  L1 Data 32K (x4)
  L1 Instruction 32K (x4)
  L2 Unified 262K (x4)
  L3 Unified 8388K (x1)
Load Average: 2.02, 8.18, 7.04
-------------------------------------------------------------------------
Benchmark                                  Time           CPU Iterations
-------------------------------------------------------------------------
BM_VirtuIterate/8                         13 ns         13 ns   50215208
BM_VirtuIterate/64                       137 ns        137 ns    4502274
BM_VirtuIterate/512                     1954 ns       1953 ns     345882
BM_VirtuIterate/4096                   26193 ns      26021 ns      26294
BM_VirtuIterate/32768                 199079 ns     198777 ns       3251
BM_VirtuIterate/262144               1994051 ns    1984474 ns        348
BM_VariantIterate/8                       13 ns         13 ns   54726835
BM_VariantIterate/64                     119 ns        118 ns    7349467
BM_VariantIterate/512                   2042 ns       2031 ns     352641
BM_VariantIterate/4096                 20883 ns      20860 ns      31584
BM_VariantIterate/32768               180835 ns     180471 ns       3872
BM_VariantIterate/262144             1698456 ns    1696731 ns        416
BM_VarvectorIterate/8                      5 ns          5 ns  161773032
BM_VarvectorIterate/64                    31 ns         31 ns   21046492
BM_VarvectorIterate/512                  263 ns        263 ns    2617967
BM_VarvectorIterate/4096                2869 ns       2868 ns     233621
BM_VarvectorIterate/32768              26434 ns      26401 ns      27608
BM_VarvectorIterate/262144            390953 ns     390151 ns       1862
BM_StableVarvectorIterate/8                9 ns          9 ns   70732077
BM_StableVarvectorIterate/64              74 ns         74 ns    8642402
BM_StableVarvectorIterate/512            602 ns        600 ns    1167425
BM_StableVarvectorIterate/4096         11821 ns      11786 ns      62434
BM_StableVarvectorIterate/32768       126536 ns     126454 ns       5067
BM_StableVarvectorIterate/262144     1079242 ns    1078489 ns        654
BM_VariantPushBack/8                   10121 ns      10112 ns      70650
BM_VariantPushBack/64                  15675 ns      15642 ns      48537
BM_VariantPushBack/512                 56868 ns      56576 ns      10000
BM_VariantPushBack/4096               393460 ns     393025 ns       2441
BM_VariantPushBack/32768             3111583 ns    3106726 ns        307
BM_VariantPushBack/262144           24685255 ns   24649973 ns         37
BM_VarvectorPushBack/8                 10230 ns      10193 ns      70185
BM_VarvectorPushBack/64                13347 ns      13271 ns      49285
BM_VarvectorPushBack/512               37916 ns      37836 ns      15824
BM_VarvectorPushBack/4096             293988 ns     293578 ns       3494
BM_VarvectorPushBack/32768           2411683 ns    2409333 ns        285
BM_VarvectorPushBack/262144         15992454 ns   15979642 ns         53
BM_StableVarvectorPushBack/8           10012 ns      10000 ns      68262
BM_StableVarvectorPushBack/64          13554 ns      13534 ns      52397
BM_StableVarvectorPushBack/512         40943 ns      40860 ns      15607
BM_StableVarvectorPushBack/4096       288718 ns     288374 ns       2735
BM_StableVarvectorPushBack/32768     2193981 ns    2191610 ns        413
BM_StableVarvectorPushBack/262144   17388039 ns   17377500 ns         46
```