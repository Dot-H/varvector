#pragma once

#include <vector>


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
        } else {
            switch_i<N+1>(tpl, n, i, fn);
        }
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

        for (const uint8_t i_vec : insertionsOrder) {
            details::switch_i<0>(varvector<Ts...>::vecs,
                                 i_vec, curIdx[i_vec], fn);
            curIdx[i_vec]++;
        }
    }

    template <typename Func>
    void foreach(Func fn) const {
        std::size_t curIdx[sizeof...(Ts)] = { 0 };

        for (const uint8_t i_vec : insertionsOrder) {
            details::switch_i<0>(varvector<Ts...>::vecs,
                                 i_vec, curIdx[i_vec], fn);
            curIdx[i_vec]++;
        }
    }

private:
    std::vector<bool> insertionsOrder;
};