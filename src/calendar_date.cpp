//
// Created by Simon Farre on 2021-02-07.
//
#include "../include/statsdb/calendar_date.hpp"
#include <iostream>

bool operator<(const CalendarDate& lhs, const CalendarDate& rhs) {

    if(lhs.year < rhs.year) {
        return true;
    } else if(lhs.year == rhs.year && lhs.month < rhs.month) {
        return true;
    } else if(lhs.year == rhs.year && lhs.month == rhs.month && lhs.day < rhs.day) {
        return true;
    } else {
        return false;
    }

}

bool operator>(const CalendarDate& lhs, const CalendarDate& rhs) {
    return !(lhs < rhs);
}

std::ostream& operator<<(std::ostream& os, const CalendarDate& c) {
    os << c.year << "-" << c.month << "-" << c.day;
    return os;
}

bool operator==(const CalendarDate& lhs, const CalendarDate& rhs) {
    return (lhs.year == rhs.year && lhs.month == rhs.month && lhs.day == rhs.day);
}
