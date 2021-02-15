//
// Created by 46769 on 2021-02-11.
//

#pragma once
#include <cassert>

namespace utils {
    template<typename T>
    struct Optional {
        std::optional<T> value{};

        template<typename Fn>
        using MapResult = Optional<typename std::invoke_result<Fn, T>::type>;

        template<typename Fn>
        auto and_then(Fn fn) -> MapResult<Fn> {
            if (value)
                return {fn(value.value())};
            else
                return {};
        }

        auto unwrap_or(T&& t) {
            if (value)
                return value.value();
            else
                return t;
        }

        constexpr bool has_value() const { return value.has_value(); }
        constexpr T& get_value() { return value.value(); }
    };

    using WSize = std::size_t;

    template<typename Container>
    concept IterableContainer = requires(Container c) {
        c.begin();
        c.cbegin();
        c.end();
        c.cend();
        typename Container::value_type;
        c.size();
    };

    template<typename Number>
    concept FloatingPoint = requires(Number num) {
        std::is_floating_point_v<Number>;
    };

    /**
     * Helper function. To keep myself from the insanity of typing out the static_cast calls everywhere.
     */
    template<typename TValue, typename SpanValue>
    inline double avg(TValue val, SpanValue sp) {
        return static_cast<double>(val) / static_cast<double>(sp);
    }

    template<IterableContainer C, typename OutputIt, typename RangeFn, typename T = typename OutputIt::container_type::value_type>
    void container_window_acc(const C &c, std::size_t window_size, OutputIt out_iterator, RangeFn fn) {
        assert(window_size <= c.size());
        auto begin = c.cbegin();
        auto container_end = c.cend();
        auto window_end = c.cbegin() + window_size;
        for (; window_end <= container_end; window_end++, begin++) {
            auto v = std::accumulate(begin, window_end, T{}, fn);
            *out_iterator = std::move(v);
        }
    }

    template<typename AccFn, typename ElType>
    concept AverageFn = requires(AccFn f) {
        f(float{}, ElType{});
    };

    template<class ElementType, std::size_t SpanWidth>
    [[nodiscard]] constexpr auto slide(std::span<ElementType, SpanWidth> span, std::size_t offset, std::size_t width) {
        return span.subspan(offset, offset + width <= span.size() ? width : 0U);
    }

    template<typename C, typename AccFn>
    auto accumulate(C c, AccFn fn) {
        using T = typename C::reference;
        if constexpr(std::is_integral_v<typename std::decay<T>::type>) {
            return std::accumulate(c.begin(), c.end(), static_cast<float>(T{}), fn);
        } else {
            return std::accumulate(c.begin(), c.end(), T{}, fn);
        }
    }
    /// Calculates the rolling span average, of a span of window_size, across the elements in c
    template<typename Number, IterableContainer C, typename AccFn>
    auto window_average(const C &container, std::size_t window_size, AccFn fn) {
        assert(window_size <= container.size());
        std::vector<Number> results;
        std::span season_span{container};
        for (std::size_t offset = 0;; ++offset) {
            auto game_span = slide(season_span, offset, window_size);
            if (game_span.empty()) break;
            float value = accumulate(game_span, fn);
            Number res = float(value / window_size);
            results.push_back(res);
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
        for (; window_end <= end; window_end++, begin++) {
            auto v = std::accumulate(begin, window_end, T{}, acc_fn);
            *out_iterator = std::move(v);
        }
    }

    template<typename Iterator, typename OutputIt, typename AccumulatorFn, typename MapFn, typename T = typename OutputIt::container_type::value_type>
    void accumulate_window_transform(Iterator begin, Iterator end, WSize window_size, OutputIt out_iterator, AccumulatorFn r_fn, MapFn map_to) {
        assert(static_cast<unsigned int>(window_size) <= (end - begin));
        auto window_end = begin + window_size;
        for (; window_end <= end; window_end++, begin++) {
            auto v = std::accumulate(begin, window_end, T{}, r_fn);
            auto tv = map_to(v);
            *out_iterator = std::move(tv);
        }
    }

}// namespace utils