//
// Created by Simon Farre on 2021-02-07.
//
#include "../include/statsdb/calendar_date.hpp"
#include <array>
#include <charconv>
#include <iosfwd>

bool operator<(const CalendarDate &lhs, const CalendarDate &rhs) {

    if (lhs.year < rhs.year) {
        return true;
    } else if (lhs.year == rhs.year && lhs.month < rhs.month) {
        return true;
    } else if (lhs.year == rhs.year && lhs.month == rhs.month && lhs.day < rhs.day) {
        return true;
    } else {
        return false;
    }
}

bool operator>(const CalendarDate &lhs, const CalendarDate &rhs) {
    return !(lhs < rhs);
}

std::ostream &operator<<(std::ostream &os, const CalendarDate &c) {
    os << c.year << "-" << c.month << "-" << c.day;
    return os;
}

bool operator==(const CalendarDate &lhs, const CalendarDate &rhs) {
    return (lhs.year == rhs.year && lhs.month == rhs.month && lhs.day == rhs.day);
}
std::string CalendarDate::to_string() {
    std::array<char, 10> output{
            '-',
            '-',
            '-',
            '-',
            '-',
            '0',
            '-',
            '-',
            '0',
            '-',
    };
    constexpr auto digits_info = [](auto ptr, auto num) {
        struct T {
            decltype(ptr) begin;
            int sz;
        };
        if (num < 10) {
            return T{ ptr + 2, 1 };
        } else {
            return T{ ptr + 1, 2 };
        }
    };
    if (auto [m_ptr, err_code] = std::to_chars(output.data(), output.data() + 4, year); err_code == std::errc()) {
        auto [month_begin, month_digits] = digits_info(m_ptr, month);
        if (auto [d_ptr, err_code] = std::to_chars(month_begin, month_begin + month_digits, month); err_code == std::errc()) {
            auto [day_begin, day_digits] = digits_info(d_ptr, day);
            if (auto [end, err_code] = std::to_chars(day_begin, day_begin + day_digits, day); err_code == std::errc()) {
                std::string_view foo{output.data(), end};
                println("Contents: '{}'", foo);
                std::string s{foo};
                return s;
            }
        }
    }
    return "";
    // return fmt::format("{}-{:0>2}-{:0>2}", year, month, day);
}
