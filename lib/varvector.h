#pragma once

#include <vector>
#include "compact_idx.h"

/*********************************************
 *             varvector details             *
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

template <std::uint8_t N, typename Tuple, typename Func>
constexpr void foreachcont_impl(Tuple& tpl, Func fn) {
    if constexpr (N < std::tuple_size_v<Tuple>) {
        const auto& container = std::get<N>(tpl);
        for (auto&& el : container) {
            fn(el);
        }
        foreachcont_impl<N+1>(tpl, fn);
    }
}

template <std::uint8_t N, typename Tuple, typename Func>
constexpr void foreach_impl(Tuple& tpl, Func fn) {
    if constexpr (N < std::tuple_size_v<Tuple>) {
        fn(std::get<N>(tpl));
        foreach_impl<N+1>(tpl, fn);
    }
}

template <std::uint8_t N, typename Tuple, typename Func>
constexpr void switch_i(Tuple& tpl, std::uint8_t nvec, std::size_t i, Func fn) {
    if constexpr (N < std::tuple_size_v<Tuple>) {
        if (nvec == N) {
            fn(std::get<N>(tpl)[i]);
        } else {
            switch_i<N+1>(tpl, nvec, i, fn);
        }
    }
}

} // namespace details

/*********************************************
 *          varvector implementation         *
 *********************************************/

template <typename... Ts>
class varvector {
public:

    template <typename Func>
    constexpr void foreach(Func fn) {
        details::foreachcont_impl<0>(_vecs, fn);
    }

    template <typename Func>
    constexpr void foreach(Func fn) const {
        details::foreachcont_impl<0>(_vecs, fn);
    }

    constexpr void reserve(std::size_t n) {
        details::foreach_impl<0>(_vecs, [&n](auto&& v) {
            v.reserve(n);
        });
    }

    template <typename T>
    constexpr void push_back(T&& value) {
        std::get<details::find_T<T, Ts...>()>(_vecs)
            .push_back(std::forward<T>(value));
    }

    template <typename T, typename... Args>
    constexpr void emplace_back(Args&&... args) {
        std::get<details::find_T<T, Ts...>()>(_vecs)
            .emplace_back(std::forward<Args>(args)...);
    }

    constexpr std::size_t size() const {
        std::size_t n = 0;
        details::foreach_impl<0>(_vecs, [&n](auto&& v) {
            n += v.size();
        });

        return n;
    }

    template <typename T>
    constexpr auto back()
        -> typename std::vector<T>::reference {
        return std::get<details::find_T<T, Ts...>()>(varvector<Ts...>::_vecs)
                    .back(); 
    }

    template <typename T>
    constexpr auto back() const 
        -> typename std::vector<T>::const_reference {
        return std::get<details::find_T<T, Ts...>()>(varvector<Ts...>::_vecs)
                    .back(); 
    }

protected:
    std::tuple<std::vector<Ts>...> _vecs;
};

/*********************************************
 *      stable_varvector implementation      *
 *********************************************/

template <typename... Ts>
struct stable_varvector : public varvector<Ts...> {
public:
    template <typename T>
    constexpr void push_back(T&& value) {
        _registerPosition<T>();
        varvector<Ts...>::push_back(std::forward<T>(value));
    }

    template <typename T, typename... Args>
    constexpr void emplace_back(Args&&... args) {
        _registerPosition<T>();
        varvector<Ts...>::template emplace_back<T>(std::forward<Args>(args)...);
    }

    constexpr void reserve(std::size_t n) {
        insertionsOrder.reserve(n);
        varvector<Ts...>::reserve(n);
    }

    template <typename Func>
    void foreach(Func fn) {
        std::size_t curIdx[sizeof...(Ts)] = { 0 };

        for (const std::uint8_t i_vec : insertionsOrder) {
            details::switch_i<0>(varvector<Ts...>::_vecs,
                                 i_vec, curIdx[i_vec]++, fn);
        }
    }

    template <typename Func>
    void foreach(Func fn) const {
        std::size_t curIdx[sizeof...(Ts)] = { 0 };

        for (const std::uint8_t i_vec : insertionsOrder) {
            details::switch_i<0>(varvector<Ts...>::_vecs,
                                 i_vec, curIdx[i_vec]++, fn);
        }
    }

private:
    std::vector<std::uint8_t> insertionsOrder;

    template <typename T>
    constexpr void _registerPosition() {
        constexpr std::uint8_t val = details::find_T<T, Ts...>();
        insertionsOrder.push_back(val);
    }
};