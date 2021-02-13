//
// Created by 46769 on 2021-02-11.
//

#include "game_data.hpp"
#include <string_view>
#include <vector>

using Games = const std::vector<Game>&;
using GamesMut = std::vector<Game>&;

/// The wonderful magic of constexpr. pow_10<3> compiles into: "mov eax, 1000" which is exactly what we want
template <int Exponent>
constexpr auto pow_10() {
    auto r = 1;
    for(auto i = 0; i < Exponent; i++) r *= 10;
    return r;
}

template <int DecimalPoints>
struct DecimalNumber {
    constexpr DecimalNumber() : value_{0.0f} {}
    constexpr DecimalNumber(float f) {
        float pow = pow_10<DecimalPoints>();
        float tmp = std::round(f * pow);
        value_ = float(tmp) / pow;
    }
    constexpr operator float() { return value_; }
    constexpr float value() { return value_; }
    float value_;
};


template <typename T>
struct StatPerPeriod {
    T period[3];
};
// Typedefs/using aliases, which facilitates refactoring much cleaner and easier
using StandardResult = DecimalNumber<2>;
using PeriodsResult = StatPerPeriod<StandardResult>;
using RollingStandard = std::vector<StandardResult>;
using RollingPeriod = std::vector<PeriodsResult>;


struct Attempts {
    int total{0};
    int results{0};
};

namespace total {
    /// Returns games where opponents pulled goalie, and where team scored in empty net
    /// Note that these stats do _not_ take into account, when the team is 5v4, and the opposing team pull goalie
    /// to be 5v5, for reasons related (currently) to scraping complexity
    Attempts empty_net_goals(std::string_view team, Games games);
    Attempts empty_net_letups(std::string_view team, Games games);
    // irrelevant under Corona pandemic. The damn teams only play against same division opponents
    Attempts outcomes_against_division(std::string_view team, Games games);
    Attempts games_with_pp_goals(std::string_view team, Games games);
    Attempts games_with_pk_letups(std::string_view team, Games games);
}

/// Where we ask questions like "What has happenened in prior games at time T, with standing S"
namespace live {
    Attempts wins_after_standing(std::string_view team, Games games);

}

/// Pre-condition of all functions: games.size() >= span. Assertion in debug, in release mode, you're screwed if you don't make sure of this.
namespace span_avg {
    RollingStandard goals_for(std::string_view team, Games games, int span);
    RollingStandard goals_against(std::string_view team, Games games, int span);
    RollingStandard shots_for(std::string_view team, Games games, int span);
    RollingStandard shots_against(std::string_view team, Games games, int span);

    RollingStandard power_play(std::string_view team, Games games, int span);
    RollingStandard penalty_kill(std::string_view team, Games games, int span);
    RollingStandard times_in_pk(std::string_view team, const std::vector<Game>&attemptsPerGame, int span);
    RollingStandard times_in_pp(std::string_view team, Games games, int span);

    RollingPeriod period_goals_for(std::string_view team, Games games, int span);
    RollingPeriod period_goals_against(std::string_view team, Games games, int span);

    RollingStandard games_with_pp_goals(std::string_view team, Games games, int span);
    RollingStandard games_with_pk_letups(std::string_view team, Games games, int span);

}

