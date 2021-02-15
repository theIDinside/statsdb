//
// Created by Simon Farre: 2021-02-15.
// Github: https://github.com/theIDinside
//

#pragma once
#include <cassert>
#include <fmt/format.h>

enum class Versus {
    Home,
    Away,
    Err
};

using CDate = CalendarDate;
struct GameInfo {
    u32 game_id;
    std::string home, away;
    CDate date;
    // clang-format off
    [[nodiscard]]
    std::string to_string() const {// clang-format on
        return fmt::format("{} - {} vs {} played {}-{:0>2}-{:0>2}", game_id, away, home, date.year, date.month, date.day);
    }
    // clang-format off
    [[nodiscard]]
    Versus venue(std::string_view team) const {// clang-format on
        if (home == team) {
            return Versus::Home;
        }
        if (away == team) {
            return Versus::Away;
        }
        assert(false && "You passed team to game where team did not play");
        return Versus::Err;
    }
};