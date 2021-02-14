//
// Created by 46769 on 2021-02-14.
//

#pragma once
#include <cstdint>
#include <utility>

template<typename It>
concept Iterator = requires(It it) {
    std::distance(It{}, It{});
    std::advance(it, 10);
    it++;
    ++it;
    *it;
};

template<Iterator Iter>
class Enumerator {
public:
    using reference = typename std::iterator_traits

    explicit Enumerator(Iter it) noexcept : iterator(std::move(it)) {}
private:
    Iter iterator;
    std::size_t index;
};