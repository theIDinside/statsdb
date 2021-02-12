//
// Created by 46769 on 2021-02-11.
//

#include "../include/statsdb/stats.hpp"
// include, what is forward declared in stats.hpp (Game)
#include "../include/statsdb/game_data.hpp"
#include "../include/statsdb/utils.hpp"

// Sys headers
#include <algorithm>
#include <cassert>


std::vector<DecimalNumber<2>> span_avg::goals_for(std::string_view team, const std::vector<Game> &games, int span) {
    assert(games.size() >= span);
    std::vector<int> goals;
    std::transform(games.cbegin(), games.cend(), std::back_inserter(goals), [&](const Game &game) {
        auto res = 0;
        for(const Goal& goal : game.goals) {
            if (goal.scoring_team == team) {
                if (goal.strength != TeamStrength::SHOOTOUT) res++;
            }
        }
        return res;
    });
    auto fn = [](float acc, int goalsPerGame) {
      return acc + float(goalsPerGame);
    };
    return utils::window_average<DecimalNumber<2>>(goals, static_cast<std::size_t>(span), +fn);
}

std::vector<DecimalNumber<2>> span_avg::power_play(std::string_view team, const std::vector<Game> &games, int span) {
    assert(games.size() >= span);
    std::vector<SpecialTeams> pps;

    for(const auto& g : games) {

    }

    std::transform(games.cbegin(), games.cend(), std::back_inserter(pps), [&](const Game& game) {
        if(game.game_info.home == team) {
            return game.power_play.home;
        } else {
            return game.power_play.away;
        }
    });
    auto begin = pps.cbegin();
    auto end = pps.cend();
    auto window_end = begin + span;
    std::vector<DecimalNumber<2>> result;
    for(; window_end <= end; window_end++, begin++) {
        auto pp = std::accumulate(begin, window_end, SpecialTeams{.goals = 0,.attempts = 0}, [](auto acc, auto v) {
            acc.goals += v.goals;
            acc.attempts += v.attempts;
            return acc;
        });
        result.emplace_back(pp.get_efficiency(SpecialTeams::Type::PowerPlay) * 100.0f);
        if(window_end == end) break;
    }
    return result;
}

std::vector<DecimalNumber<2>> span_avg::penalty_kill(std::string_view team, const std::vector<Game> &games, int span) {
    assert(games.size() >= span);
    std::vector<SpecialTeams> pps;
    std::transform(games.cbegin(), games.cend(), std::back_inserter(pps), [&](const Game& game) {
      if(game.game_info.home == team) {
          return game.power_play.away;
      } else {
          return game.power_play.home;
      }
    });
    auto begin = pps.cbegin();
    auto end = pps.cend();
    auto window_end = begin + span;
    std::vector<DecimalNumber<2>> result;
    for(; window_end <= end; window_end++, begin++) {
        auto pp = std::accumulate(begin, window_end, SpecialTeams{.goals = 0,.attempts = 0}, [](auto acc, auto v) {
          acc.goals += v.goals;
          acc.attempts += v.attempts;
          return acc;
        });
        result.emplace_back(pp.get_efficiency(SpecialTeams::Type::PenaltyKilling) * 100.0f);
        if(window_end == end) break;
    }
    return result;
}
