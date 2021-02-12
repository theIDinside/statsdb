//
// Created by 46769 on 2021-02-11.
//

#pragma once
#include <vector>
#include <numeric>
#include <cassert>

namespace utils {
    using WSize = std::size_t;

    template <typename Container> concept IterableContainer = requires(Container c) {
        c.begin();
        c.cbegin();
        c.end();
        c.cend();
        typename Container::value_type;
        c.size();
    };

    template <typename Number> concept FloatingPoint = requires(Number num) {
            std::is_floating_point_v<Number>;
    };

    /**
     * Helper function. To keep myself from the insanity of typing out the static_cast calls everywhere.
     */
    template <typename TValue, typename SpanValue>
    inline double avg(TValue val, SpanValue sp) {
        return static_cast<double>(val) / static_cast<double>(sp);
    }

    template<IterableContainer C, typename OutputIt, typename RangeFn, typename T = typename OutputIt::container_type::value_type>
    void container_window_acc(const C& c, std::size_t window_size, OutputIt out_iterator, RangeFn fn) {
        assert(window_size <= c.size());
        auto begin = c.cbegin();
        auto container_end = c.cend();
        auto window_end = c.cbegin() + window_size;
        for(; window_end <= container_end; window_end++, begin++) {
            auto v = std::accumulate(begin, window_end, T{}, fn);
            *out_iterator = std::move(v);
        }
    }

    template <typename AccFn, typename ElType> concept AverageFn = requires(AccFn f) {
        f(float{}, ElType{});
    };

    template <typename Number, IterableContainer C, typename AccFn>
    auto window_average(const C& c, std::size_t window_size, AccFn fn) {
        assert(window_size <= c.size());

        auto begin = c.cbegin();
        auto end = c.cend();
        auto window_end = begin + window_size;
        std::vector<Number> results;
        for(; window_end <= end; window_end++, begin++) {
            auto value = 0.0f;
            for(auto start = begin; start != window_end; start++) {
                auto tmp = *start;
                value += tmp;
            }
            Number res = float(value / window_size);
            results.push_back(res);
            if(window_end == end) break;
        }
        return results;
    }

    /*
     * Function that takes two iterators, begin and end, and a size, the sliding range between them, that accumulate will be
     * ran on. Useful for calculating averages over a span. out_iterator must be an output iterator that can output the accumulated
     * value of the window_sized range. RangeFn is the function that is ran on each individual element in the sub ranges/sliding window element,
     * and therefore RangeFn must be a lambda/function with a parameter of T, here defined as the value_type of the output iterators container's value_type,
     * and the element type.
     */
    template<typename Iterator, typename OutputIterator, typename AccumulatorFn, typename T = typename OutputIterator::container_type::value_type>
    void accumulate_windows(Iterator begin, Iterator end, std::size_t window_size, OutputIterator out_iterator, AccumulatorFn acc_fn) {
        assert(static_cast<unsigned int>(window_size) <= (end - begin));
        auto window_end = begin + window_size;
        for(; window_end <= end; window_end++, begin++) {
            auto v = std::accumulate(begin, window_end, T{}, acc_fn);
            *out_iterator = std::move(v);
        }
    }

    template<typename Iterator, typename OutputIt, typename AccumulatorFn, typename MapFn, typename T = typename OutputIt::container_type::value_type>
    void accumulate_window_transform(Iterator begin, Iterator end, WSize window_size, OutputIt out_iterator, AccumulatorFn r_fn, MapFn map_to) {
        assert(static_cast<unsigned int>(window_size) <= (end - begin));
        auto window_end = begin + window_size;
        for(; window_end <= end; window_end++, begin++) {
            auto v = std::accumulate(begin, window_end, T{}, r_fn);
            auto tv = map_to(v);
            *out_iterator = std::move(tv);
        }
    }

}