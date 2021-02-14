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

#ifdef DEBUG
#define TEAM_ASSERTION(team, game) assert((game.game_info.away == team || game.game_info.home == team) && "You passed games that team did not play in. You fucked up.")

#define VERSUS_ASSERT_SWITCH(team, game) \
    TEAM_ASSERTION(team, game);          \
    switch (game.game_info.venue(team))
#define ON_ERR_EXIT                                                                                                                          \
    case Versus::Err:                                                                                                                        \
        println("You passed the wrong team name to this game: {}. Teams playing: {} vs {}", team, game.game_info.away, game.game_info.home); \
        exit(-1);
#else
#define TEAM_ASSERTION(team, game)

#define VERSUS_ASSERT_SWITCH(team, game) switch (game.game_info.venue(team))

#define ON_ERR_EXIT                                                                                                                          \
    case Versus::Err:                                                                                                                        \
        println("You passed the wrong team name to this game: {}. Teams playing: {} vs {}", team, game.game_info.away, game.game_info.home); \
        exit(-1);
#endif

namespace total {
    Attempts empty_net_goals(const std::string_view team, Games games) {
        Attempts stat;
        const auto won_game_predicate = [team](const Game &game) {
            return game.winning_team == team;
        };

        auto won_games = games | std::views::filter(won_game_predicate);
        for (const auto &g : won_games) {
            if (g.goalie_probably_pulled) [[likely]] {
                stat.total++;
                for (const auto &goal : g.goals | std::views::filter(empty_net_goal)) {
                    stat.results++;
                }
            } else {
                for (const auto &goal : g.goals | std::views::filter(empty_net_goal)) {
                    stat.total++;
                    stat.results++;
                }
            }
        }
        return stat;
    }

    Attempts empty_net_letups(const std::string_view team, const std::vector<Game> &games) {
        Attempts stat;
        auto lost_game_predicate = [&](const Game &game) {
            return game.winning_team != team;
        };

        for (const auto &g : games | std::views::filter(lost_game_predicate)) {
            if (g.goalie_probably_pulled) [[likely]] {
                stat.total++;
                for (const auto &goal : g.goals | std::views::filter(empty_net_goal)) {
                    stat.results++;
                }
            } else {
                for (const auto &goal : g.goals | std::views::filter(empty_net_goal)) {
                    stat.total++;
                    stat.results++;
                }
            }
        }
        return stat;
    }

    Attempts games_with_pp_goals(const std::string_view team, const std::vector<Game> &games) {
        Attempts stat;
        auto tfm = [&](const auto &g) {
            return get_special_teams(g, team, SpecialTeams::Type::PowerPlay);
        };

        for (const auto &pp : games | std::views::transform(tfm)) {
            stat.total++;
            if (pp.goals > 0) stat.results++;
        }
        return stat;
    }

    Attempts games_with_pk_letups(const std::string_view team, const std::vector<Game> &games) {
        Attempts stat;
        auto tfm = [&](const auto &g) {
            return get_special_teams(g, team, SpecialTeams::Type::PenaltyKilling);
        };

        for (const auto &pp : games | std::views::transform(tfm)) {
            stat.total++;
            if (pp.goals > 0) stat.results++;
        }
        return stat;
    }
}// namespace total

namespace span_avg {
    RollingStandard goals_for(const std::string_view team, Games games, int span) {
        assert(games.size() >= span);
        std::vector<int> goals;
        goals.reserve(games.size());
        std::transform(games.cbegin(), games.cend(), std::back_inserter(goals), [&](const Game &game) {
            TEAM_ASSERTION(team, game);
            auto res = 0;
            for (const Goal &goal : game.goals) {
                if (goal.scoring_team == team) {
                    if (goal.strength != TeamStrength::SHOOTOUT) res++;
                }
            }
            return res;
        });
        auto fn = [](float acc, int goalsPerGame) -> float {
            return acc + float(goalsPerGame);
        };
        return utils::window_average<RoundedDecimalNumber<2>>(goals, static_cast<std::size_t>(span), +fn);
    }

    RollingStandard goals_against(const std::string_view team, Games games, int span) {
        assert(games.size() >= span);
        std::vector<int> goals;
        goals.reserve(games.size());
        std::transform(games.cbegin(), games.cend(), std::back_inserter(goals), [&](const Game &game) {
            TEAM_ASSERTION(team, game);
            auto res = 0;
            for (const Goal &goal : game.goals) {
                if (goal.scoring_team != team) {
                    if (goal.strength != TeamStrength::SHOOTOUT) res++;
                }
            }
            return res;
        });
        return utils::window_average<RoundedDecimalNumber<2>>(goals, static_cast<std::size_t>(span), [](float acc, int goalsPerGame) {
            return acc + float(goalsPerGame);
        });
    }

    RollingStandard shots_for(const std::string_view team, Games games, int span) {
        assert(games.size() >= span);
        std::vector<int> shots_for_per_game;
        shots_for_per_game.reserve(games.size());
        std::transform(games.cbegin(), games.cend(), std::back_inserter(shots_for_per_game), [&](const Game &game) {
            VERSUS_ASSERT_SWITCH(team, game) {
                case Versus::Home:
                    return std::accumulate(game.shots.begin(), game.shots.end(), 0, [](auto acc, auto period) { return acc + period.home; });
                case Versus::Away:
                    return std::accumulate(game.shots.begin(), game.shots.end(), 0, [](auto acc, auto period) { return acc + period.away; });
                    ON_ERR_EXIT
            }
        });

        return utils::window_average<RoundedDecimalNumber<2>>(shots_for_per_game, static_cast<std::size_t>(span), [](float acc, int shotsPerGame) {
            return acc + float(shotsPerGame);
        });
    }
    RollingStandard shots_against(const std::string_view team, Games games, int span) {
        assert(games.size() >= span);
        std::vector<int> shots_against_per_game;
        shots_against_per_game.reserve(games.size());
        std::transform(games.cbegin(), games.cend(), std::back_inserter(shots_against_per_game), [&](const Game &game) {
            VERSUS_ASSERT_SWITCH(team, game) {
                case Versus::Home:
                    return std::accumulate(game.shots.begin(), game.shots.end(), 0, [](auto acc, auto period) { return acc + period.away; });
                case Versus::Away:
                    return std::accumulate(game.shots.begin(), game.shots.end(), 0, [](auto acc, auto period) { return acc + period.home; });
                    ON_ERR_EXIT
            }
        });

        return utils::window_average<RoundedDecimalNumber<2>>(shots_against_per_game, static_cast<std::size_t>(span), [](float acc, int shotsPerGame) {
            return acc + float(shotsPerGame);
        });
    }
    RollingStandard times_in_pk(const std::string_view team, Games games, int span) {
        assert(games.size() >= span);
        std::vector<int> times_shorthanded;
        times_shorthanded.reserve(games.size());
        std::transform(games.cbegin(), games.cend(), std::back_inserter(times_shorthanded), [&](const Game &game) {
            VERSUS_ASSERT_SWITCH(team, game) {
                case Versus::Home:
                    return game.power_play.away.attempts;
                case Versus::Away:
                    return game.power_play.home.attempts;
                    ON_ERR_EXIT
            }
        });
        return utils::window_average<RoundedDecimalNumber<2>>(times_shorthanded, static_cast<std::size_t>(span), [](float acc, int attemptsPerGame) {
            return acc + float(attemptsPerGame);
        });
    }

    RollingStandard times_in_pp(const std::string_view team, Games games, int span) {
        assert(games.size() >= span);
        std::vector<int> times_in_pp;
        times_in_pp.reserve(games.size());
        std::transform(games.cbegin(), games.cend(), std::back_inserter(times_in_pp), [&](const Game &game) {
            VERSUS_ASSERT_SWITCH(team, game) {
                case Versus::Home:
                    return game.power_play.home.attempts;
                case Versus::Away:
                    return game.power_play.away.attempts;
                    ON_ERR_EXIT
            }
        });
        return utils::window_average<RoundedDecimalNumber<2>>(times_in_pp, static_cast<std::size_t>(span), [](float acc, int attemptsPerGame) {
            return acc + float(attemptsPerGame);
        });
    }

    RollingStandard power_play(const std::string_view team, Games games, int span) {
        assert(games.size() >= span);
        std::vector<SpecialTeams> pps;
        pps.reserve(games.size());
        std::transform(games.cbegin(), games.cend(), std::back_inserter(pps), [&](const Game &game) {
            VERSUS_ASSERT_SWITCH(team, game) {
                case Versus::Home:
                    return game.power_play.home;
                case Versus::Away:
                    return game.power_play.away;
                    ON_ERR_EXIT
            }
        });
        auto begin = pps.cbegin();
        auto end = pps.cend();
        auto window_end = begin + span;
        RollingStandard result;
        for (; window_end <= end; window_end++, begin++) {
            auto pp = std::accumulate(begin, window_end, SpecialTeams{.goals = 0, .attempts = 0}, [](auto acc, auto v) {
                acc.goals += v.goals;
                acc.attempts += v.attempts;
                return acc;
            });
            result.emplace_back(pp.get_efficiency(SpecialTeams::Type::PowerPlay) * 100.0f);
            if (window_end == end) break;
        }
        return result;
    }

    RollingStandard penalty_kill(const std::string_view team, Games games, int span) {
        assert(games.size() >= span);
        std::vector<SpecialTeams> pks;
        pks.reserve(games.size());
        std::transform(games.cbegin(), games.cend(), std::back_inserter(pks), [&](const Game &game) {
            VERSUS_ASSERT_SWITCH(team, game) {
                case Versus::Home:
                    return game.power_play.away;
                case Versus::Away:
                    return game.power_play.home;
                    ON_ERR_EXIT
            }
        });
        auto begin = pks.cbegin();
        const auto end = pks.cend();
        auto window_end = begin + span;
        RollingStandard result;
        for (; window_end <= end; window_end++, begin++) {
            const auto pp = std::accumulate(begin, window_end, SpecialTeams{.goals = 0, .attempts = 0}, [](auto acc, auto v) {
                acc.goals += v.goals;
                acc.attempts += v.attempts;
                return acc;
            });
            result.emplace_back(pp.get_efficiency(SpecialTeams::Type::PenaltyKilling) * 100.0f);
            if (window_end == end) break;
        }
        return result;
    }
}// namespace span_avg

namespace span_avg::period {
    /*
    RollingPeriod goals_for(std::string_view team, const std::vector<Game> &games, int span) {
        assert(games.size() >= span);
        PeriodsResult stat;
        std::vector<int> goals;
        goals.reserve(games.size());
        auto filter_goals = [team](const auto goal) {
          return goal.scoring_team == team;
        };


        for(auto goals_per_game : games | std::views::transform([](const auto& game) { return game.goals; })) {
            for(const auto& goal : goals_per_game | std::views::filter(filter_goals)) {

            }
        }



        std::transform(games.cbegin(), games.cend(), std::back_inserter(goals), [&](const Game &game) {
          TEAM_ASSERTION(team, game);
          auto res = 0;
          for (const Goal &goal : game.goals) {
              if (goal.scoring_team == team) {
                  if (goal.strength != TeamStrength::SHOOTOUT) res++;
              }
          }
          return res;
        });
        auto fn = [](float acc, int goalsPerGame) -> float {
          return acc + float(goalsPerGame);
        };
        return utils::window_average<RoundedDecimalNumber<2>>(goals, static_cast<std::size_t>(span), +fn);
    }*/
}

namespace live {
    Attempts outcomes_after_standing(LiveParameter params, const std::vector<Game> &games) {
        Attempts stat;
        const auto &[team, score, opponent_score, time] = params;
        const auto filter = [&](const auto &g) {
            return game_had_standing(params, g);
        };
        for (const auto &g : games | std::views::filter(filter)) {
            if (g.winning_team == team) {
                stat.results++;
            }
            stat.total++;
        }
        return stat;
    }

    bool game_had_standing(LiveParameter param, const Game &game) {
        const auto &[team, score, opponent_score, time] = param;
        if (game.game_info.home == team) {
            auto home_score = 0;
            auto away_score = 0;
            for (const auto &g : game.goals) {
                if (g.scoring_team == team) home_score++;
                else
                    away_score++;
                if (home_score == score && away_score == opponent_score) return true;
                if (home_score > score || away_score > opponent_score) return false;
            }
            return false;
        } else {
            auto home_score = 0;
            auto away_score = 0;
            for (const auto &g : game.goals) {
                if (g.scoring_team == team) away_score++;
                else
                    home_score++;
                if (away_score == score && home_score == opponent_score) return true;
                if (away_score > score || home_score > opponent_score) return false;
            }
            return false;
        }
    }
}// namespace live
