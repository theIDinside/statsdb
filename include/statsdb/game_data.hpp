//
// Created by Simon Farre on 2021-02-07.
//

#pragma once
#pragma once
// local depedencies
#include "calendar_date.hpp"

// system headers
#include <string>
#include <vector>
#include <fmt/format.h>

template <typename T>
struct TeamsValueHolder {
    T home;
    T away;
};

struct Team
{
    u8 id;
    std::string name;
    std::vector<std::string> abbreviations;
    friend bool operator==(const Team& lhs, const Team& rhs);
    friend bool operator<(const Team& lhs, const Team& rhs);
    friend bool operator>(const Team& lhs, const Team& rhs);
    friend bool operator==(const Team& lhs, const std::string& team_string);
    friend bool operator==(const Team& lhs, std::string_view team_string);
};

using CDate = CalendarDate;
struct GameInfo
{
    u32      game_id;
    std::string home, away;
    CDate    date;
    [[nodiscard]] std::string to_string() const {
        return fmt::format("{} - {} vs {} played {}-{:0>2}-{:0>2}", game_id, away, home, date.year, date.month, date.day);
    }
};

struct Time
{
    u8 minutes;
    u8 seconds;
};

struct GameTime {
    Time time;
    u8 period;
};

enum class TeamStrength {
    EVEN,
    EVEN_PENALTY_SHOT,
    PENALTY_SHOT,
    EVEN_EMPTY_NET,
    POWER_PLAY,
    SHORT_HANDED,
    SHORT_HANDED_EMPTY_NET,
    SHORT_HANDED_PENALTY_SHOT,
    POWER_PLAY_EMPTY_NET,
    POWER_PLAY_PENALTY_SHOT,
    SHOOTOUT
};

struct Goal
{
    u8 goal_number;
    GameTime time;
    TeamStrength strength;
    std::string scoring_team;
    std::string scoring_player;
};

struct SpecialTeams {
    enum class Type {
        PowerPlay,
        PenaltyKilling
    };
    int goals;
    int attempts;
    double get_efficiency(Type type) const;
};

using IntResult = TeamsValueHolder<int>;
using FloatResult = TeamsValueHolder<float>;
using PowerPlay = TeamsValueHolder<SpecialTeams>;

struct Game
{
    GameInfo game_info;
    std::string winning_team;
    std::vector<IntResult> shots;
    IntResult final_result;
    FloatResult face_off;
    PowerPlay power_play;
    IntResult pim;
    IntResult hits;
    IntResult blocked_shots;
    IntResult give_aways;
    IntResult take_aways;
    std::vector<Goal> goals;
};
