//
// Created by Simon Farre on 2021-02-07.
//

#pragma once
#include "defs.hpp"
struct CalendarDate
{
    u16 year;
    u8 month;
    u8 day;
    friend bool operator<(const CalendarDate& lhs, const CalendarDate& rhs);
    friend bool operator>(const CalendarDate& lhs, const CalendarDate& rhs);
    friend bool operator==(const CalendarDate& lhs, const CalendarDate& rhs);
    std::string to_string();
};
