//
// Created by Simon Farre on 2021-02-07.
//

#pragma once
#include "defs.hpp"
#include <fmt/format.h>

struct CalendarDate
{
    u16 year{};
    u8 month{};
    u8 day{};
    friend bool operator<(const CalendarDate& lhs, const CalendarDate& rhs);
    friend bool operator>(const CalendarDate& lhs, const CalendarDate& rhs);
    friend bool operator==(const CalendarDate& lhs, const CalendarDate& rhs);
    std::string to_string();
};

template <>
struct fmt::formatter<CalendarDate>
{
    using Self = CalendarDate;
    constexpr auto parse(format_parse_context& ctx) {
        auto it = ctx.begin();
        auto end = ctx.end();
        if(it != end && *it != '}')
            throw format_error("invalid format");
        return it;
    }

    template <typename FormatContext>
    auto format(const Self& s, FormatContext& ctx) {
        return format_to(
                ctx.out(),
                "{}-{:0>2}-{:0>2}",
                s.year, s.month, s.day
        );
    }
};