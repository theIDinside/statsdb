//
// Created by Simon Farre on 2021-02-07.
//

#pragma once
#pragma once
// local depedencies
#include "calendar_date.hpp"
#include "decimal_number.hpp"
#include "game_info.hpp"

// system headers
#include <chrono>
#include <fmt/format.h>

template<typename T>
struct TeamsValueHolder {
    T home{};
    T away{};

    TeamsValueHolder difference(const TeamsValueHolder &other) {
        return TeamsValueHolder{home - other.home, away - other.away};
    }
};

struct Team {
    u8 id;
    std::string name;
    std::vector<std::string> abbreviations;
    friend bool operator==(const Team &lhs, const Team &rhs);
    friend bool operator<(const Team &lhs, const Team &rhs);
    friend bool operator>(const Team &lhs, const Team &rhs);
    friend bool operator==(const Team &lhs, const std::string &team_string);
    friend bool operator==(const Team &lhs, std::string_view team_string);
};

struct Time {
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

struct Goal {
    u8 goal_number;
    GameTime time;
    TeamStrength strength;
    std::string scoring_team;
    std::string scoring_player;
};

bool empty_net_goal(const Goal &g);

struct SpecialTeams {
    enum class Type {
        PowerPlay,
        PenaltyKilling
    };
    int goals;
    int attempts;
    // clang-format off

    template<NumberFormat format = NumberFormat::Value>
    [[nodiscard]] inline constexpr float get_efficiency(Type type) const {
        if constexpr (format == NumberFormat::Percent) {
            if(type == Type::PowerPlay) {
                return float(goals) / float(attempts) * 100.0f;
            } else if(type == Type::PenaltyKilling) {
                return (1.0f - float(goals) / float(attempts)) * 100.0f;
            }
        } else {
            if(type == Type::PowerPlay) {
                return float(goals) / float(attempts);
            } else if(type == Type::PenaltyKilling) {
                return 1.0f - float(goals) / float(attempts);
            }
        }
    }

    friend SpecialTeams operator+(const SpecialTeams& left, const SpecialTeams& right);
};

using IntResult = TeamsValueHolder<int>;
using FloatResult = TeamsValueHolder<float>;
using PowerPlay = TeamsValueHolder<SpecialTeams>;


struct Game {
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
    bool goalie_probably_pulled;
};

struct StatsAverage {
    FloatResult goals{};
    FloatResult shots{};
    FloatResult pp_efficiency{};
    FloatResult pk_efficiency{};
    FloatResult pp{};
    FloatResult pk{};
    FloatResult pim{};
    FloatResult hits{};
    FloatResult face_off{};
    FloatResult blocked_shots{};
    FloatResult give_aways{};
    FloatResult take_aways{};
    void add_game(const Game &g, bool isHomeTeam);
    friend StatsAverage operator/(const StatsAverage &acc, float divisor);
    void print() const;
    StatsAverage difference(const StatsAverage &other);
};

struct GameStatsAccumulator {
    int games_accumulated = 0;
    float goals{};
    float shots{};
    SpecialTeams pp{};
    SpecialTeams pk{};
    float pim{};
    float hits{};
    float face_off{};
    float blocked_shots{};
    float give_aways{};
    float take_aways{};
    void add_game(const Game &g, bool isHomeTeam);
    friend GameStatsAccumulator operator/(const GameStatsAccumulator &acc, float divisor);
};

struct AverageInfo {
    std::string team_name;
    GameStatsAccumulator data;
};

void print_comparison(const AverageInfo& home, const AverageInfo& away);
SpecialTeams get_special_teams(const Game &game, std::string_view team, SpecialTeams::Type type);