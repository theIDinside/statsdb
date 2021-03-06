//
// Created by 46769 on 2021-02-07.
//
#include <array>
#include <cassert>
#include <iostream>
#include <numeric>
#include <statsdb/stats.hpp>
#include <statsdb/statsdb.h>

struct StandingStats {
    CalendarDate date{2021, 2, 16};
    int won{};
    int loss{};
    int ot_loss{};
    int points{};
    float points_pct{};
    int regular_wins{};
    int ot_wins{};
    int so_wins{};
    int gf{};
    int ga{};
    float gf_avg{};
    float ga_avg{};
    float pp{};
    float pk{};
    float shots_per_game{};
    float shots_against_per_game{};
};

template<>
struct fmt::formatter<StandingStats> {
    using Self = StandingStats;
    char presentation = 'f';
    constexpr auto parse(format_parse_context &ctx) {
        auto it = ctx.begin();
        auto end = ctx.end();
        if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;
        if (it != end && *it != '}')
            throw format_error("invalid format");
        return it;
    }

    template<typename FormatContext>
    auto format(const Self &s, FormatContext &ctx) {
        return format_to(
                ctx.out(),
                R"(Date: {}
Won: {}
Lost: {}
OT Losses: {}
Pts: {}
Pts per game {}
Regulation wins: {}
OT Wins: {}
SO Wins: {}
Goals for: {}
Goals against: {}
Goals for, game avg: {}
Goals against, game avg: {}
PP Efficiency: {}%
PK Efficiency: {}%
Shots for, game avg: {}
Shots against, game avg: {})",
                s.date,
                s.won,
                s.loss,
                s.ot_loss,
                s.points,
                s.points_pct,
                s.regular_wins,
                s.ot_wins,
                s.so_wins,
                s.gf,
                s.ga,
                s.gf_avg,
                s.ga_avg,
                s.pp,
                s.pk,
                s.shots_per_game,
                s.shots_against_per_game);
    }
};


StandingStats TorontoTeamStanding8thFeb{
        .date = CalendarDate{2021, 2, 8},
        .won = 9,    //
        .loss = 2,   //
        .ot_loss = 1,//
        .points = 19,//
        .points_pct = 0.792f,
        .regular_wins = 8,//
        .ot_wins = 1,     //
        .so_wins = 0,     //
        .gf = 45,
        .ga = 33,
        .gf_avg = 3.75f,//
        .ga_avg = 2.75f,//
        .pp = 38.5f,
        .pk = 76.1f,
        .shots_per_game = 30.6f,
        .shots_against_per_game = 26.9f};

StandingStats TorontoTeamStanding11thFeb{
        .date = CalendarDate{2021, 2, 11},
        .won = 11,   //
        .loss = 2,   //
        .ot_loss = 1,//
        .points = 23,//
        .points_pct = 0.821f,
        .regular_wins = 10,//
        .ot_wins = 1,      //
        .so_wins = 0,      //
        .gf = 52,
        .ga = 36,
        .gf_avg = 3.71f,//
        .ga_avg = 2.57f,//
        .pp = 34.9f,
        .pk = 76.9f,
        .shots_per_game = 29.3f,
        .shots_against_per_game = 27.9f};

StandingStats BostonTeamStanding11thFeb{
        .date = CalendarDate{2021, 2, 11},
        .won = 9,    //
        .loss = 1,   //
        .ot_loss = 2,//
        .points = 20,//
        .points_pct = 0.833f,
        .regular_wins = 4,//
        .ot_wins = 3,     //
        .so_wins = 2,     //
        .gf = 37,
        .ga = 26,
        .gf_avg = 3.08f,//
        .ga_avg = 2.17f,//
        .pp = 31.6f,
        .pk = 88.4f,
        .shots_per_game = 31.9f,
        .shots_against_per_game = 24.4f};


auto make_toronto_tests() {
    return std::array<StandingStats, 2>{
            TorontoTeamStanding8thFeb, TorontoTeamStanding11thFeb};
}

auto make_boston_tests() {
    return std::array<StandingStats, 1>{BostonTeamStanding11thFeb};
}


template<typename T>
float round_to_decimal(T value, int decimal_points) {
    double pow = std::pow(10, decimal_points);
    auto tmp = std::round(value * pow);
    auto res = static_cast<float>(tmp) / static_cast<float>(pow);
    return res;
}


void test_get_toronto_played_7th_feb(Database &db) {
    auto games = db.get_games_played_by("TOR");
    games->erase(std::remove_if(games->begin(), games->end(), [](auto g) {
                     return g.game_info.date > CalendarDate{.year = 2021, .month = 2, .day = 7};
                 }),
                 games->end());

    assert(games->size() == 12);
}

void test_get_toronto_wins(Database &db) {
    auto games = db.get_games_played_by("TORONTO MAPLE LEAFS");
    games->erase(std::remove_if(games->begin(), games->end(), [](auto g) {
                     return g.game_info.date > CalendarDate{.year = 2021, .month = 2, .day = 7};
                 }),
                 games->end());
    if (games) {
        auto gs = games.value();
        auto won = std::accumulate(gs.begin(), gs.end(), 0, [](auto acc, auto g) {
            if (g.winning_team == "TOR") return ++acc;
            return acc;
        });
        assert(won == 9);
        println("Toronto won games: {}", won);
    }
}

void test_toronto_goals(Database &db) {
    constexpr auto gf = 3.75;
    constexpr auto ga = 2.75;
    auto games = db.get_games_played_by("TORONTO MAPLE LEAFS");
    games->erase(std::remove_if(games->begin(), games->end(), [](auto g) {
                     return g.game_info.date > CalendarDate{.year = 2021, .month = 2, .day = 7};
                 }),
                 games->end());
    if (games) {
        auto gs = games.value();
        auto acc_gf = 0.0f;
        auto acc_ga = 0.0f;
        auto games_played = static_cast<double>(gs.size());
        for (auto &g : gs) {
            if (g.game_info.home == "TOR") {
                acc_gf += g.final_result.home;
                acc_ga += g.final_result.away;
            } else {
                acc_gf += g.final_result.away;
                acc_ga += g.final_result.home;
            }
        }

        acc_gf = round_to_decimal(acc_gf / games_played, 3);
        acc_ga = round_to_decimal(acc_ga / games_played, 3);
        assert(acc_gf == gf);
        assert(acc_ga == ga);
        println("GF: {} - GA: {} ({}/{})", acc_gf, acc_ga, gf, ga);
    }
}

void verify_toronto_team_standings(Database &db) {
    println("{}", __FUNCSIG__);
    auto tests = make_toronto_tests();
    auto all_games = db.get_games_played_by("TORONTO MAPLE LEAFS");
    for (const auto &stat : tests) {
        StandingStats acc{};
        auto date = stat.date;
        auto games = all_games;
        games->erase(std::remove_if(games->begin(), games->end(), [&](auto g) {
                         return g.game_info.date > date;
                     }),
                     games->end());

        auto avg = trend::goals_for("TOR", games.value(), games->size());
        auto pp_span = trend::power_play("TOR", games.value(), games->size());
        auto pk_span = trend::penalty_kill("TOR", games.value(), games->size());

        println("PP: {}", pp_span.back().value());
        println("Avg totals should be 1 = {}", avg.size());
        assert(avg.size() == 1);// since we take the entirety of the season, this must be 1

        if (games) {
            auto gs = games.value();
            SpecialTeams pp{};
            SpecialTeams pk{};
            auto games_played = static_cast<double>(gs.size());
            for (auto &g : gs) {
                if (g.game_info.home == "TOR") {
                    acc.gf += g.final_result.home;
                    acc.ga += g.final_result.away;
                    acc.shots_per_game += std::accumulate(g.shots.begin(), g.shots.end(), 0.0f, [](auto acc, auto period) { return acc + period.home; });
                    acc.shots_against_per_game += std::accumulate(g.shots.begin(), g.shots.end(), 0.0f, [](auto acc, auto period) { return acc + period.away; });
                    pp.goals += g.power_play.home.goals;
                    pp.attempts += g.power_play.home.attempts;
                    pk.goals += g.power_play.away.goals;
                    pk.attempts += g.power_play.away.attempts;

                } else {
                    acc.gf += g.final_result.away;
                    acc.ga += g.final_result.home;
                    acc.shots_per_game += std::accumulate(g.shots.begin(), g.shots.end(), 0.0f, [](auto acc, auto period) { return acc + period.away; });
                    acc.shots_against_per_game += std::accumulate(g.shots.begin(), g.shots.end(), 0.0f, [](auto acc, auto period) { return acc + period.home; });
                    pp.goals += g.power_play.away.goals;
                    pp.attempts += g.power_play.away.attempts;
                    pk.goals += g.power_play.home.goals;
                    pk.attempts += g.power_play.home.attempts;
                }

                if (g.winning_team == "TOR") {
                    if (auto last_goal_period = g.goals.back().time.period; last_goal_period < 4) {
                        acc.regular_wins++;
                    } else {
                        if (last_goal_period == 4) {
                            acc.ot_wins++;
                        } else if (last_goal_period == 5) {
                            acc.so_wins++;
                            acc.gf--;// shoot-out goals don't end up in the GF stats, nor the GA stats
                        }
                    }
                    acc.won++;
                    acc.points += 2;
                } else {
                    if (auto last_goal_period = g.goals.back().time.period; last_goal_period < 4) {
                        acc.loss++;
                    } else {
                        if (last_goal_period == 5) {
                            acc.ga--;
                        }
                        acc.ot_loss++;
                        acc.points += 1;
                    }
                }
            }
            println("Assert {} == {}", round_to_decimal(avg.back(), 2), stat.gf_avg);
            assert(round_to_decimal(avg.back(), 2) == stat.gf_avg);
            acc.points_pct = round_to_decimal(acc.points / (games_played * 2), 3);
            acc.pp = round_to_decimal((float(pp.goals) / float(pp.attempts)) * 100.0, 1);
            auto tmp = (round_to_decimal((1.0 - float(pk.goals) / float(pk.attempts)) * 100.0, 1));
            acc.pk = tmp;
            acc.gf_avg = round_to_decimal(acc.gf / games_played, 2);
            acc.ga_avg = round_to_decimal(acc.ga / games_played, 2);
            acc.shots_against_per_game = round_to_decimal(acc.shots_against_per_game / games_played, 1);
            acc.shots_per_game = round_to_decimal(acc.shots_per_game / games_played, 1);
            println("Assert: {} == {}", round_to_decimal(pp_span.back().value(), 1), acc.pp);
            assert(round_to_decimal(pp_span.back().value(), 1) == acc.pp);
        }
        println("Won: {}/{}\nLoss: {}/{}\nOT LOSS: {}/{}\nPoints: {}/{}\nPoints pct: {}/{}\nRegular wins: {}/{}\nOT Wins: {}/{}\nSO Wins: {}/{}\nGF: {}/{}\nGA: {}/{}\nGFA: {}/{}\nGAA: {}/{}\nPP: {}/{}\nPK: {}/{}\nShots: {}/{}\nShots against: {}/{}\n",
                acc.won, stat.won,
                acc.loss, stat.loss,
                acc.ot_loss, stat.ot_loss,
                acc.points, stat.points,
                acc.points_pct, stat.points_pct,
                acc.regular_wins, stat.regular_wins,
                acc.ot_wins, stat.ot_wins,
                acc.so_wins, stat.so_wins,
                acc.gf, stat.gf,
                acc.ga, stat.ga,
                acc.gf_avg, stat.gf_avg,
                acc.ga_avg, stat.ga_avg,
                acc.pp, stat.pp,
                round_to_decimal(pk_span.back().value(), 1), stat.pk,
                acc.shots_per_game, stat.shots_per_game,
                acc.shots_against_per_game, stat.shots_against_per_game);
    }
}
void verify_boston_team_standings(Database &db) {
    println("Test {}", __FUNCSIG__);
    auto game = db.get_game(2020020212);
    assert(game.has_value());
    auto all_games = db.get_games_played_by("BOSTON BRUINS");
    auto tests = make_boston_tests();
    for (const auto &stat : tests) {
        StandingStats acc{};
        auto games = all_games;
        auto date = stat.date;
        games->erase(std::remove_if(games->begin(), games->end(), [&](auto g) {
                         return g.game_info.date > date;
                     }),
                     games->end());
        assert(games->size() == 12);
        if (games) {
            auto gs = games.value();
            SpecialTeams pp{};
            SpecialTeams pk{};
            auto games_played = static_cast<double>(gs.size());
            for (auto &g : gs) {
                if (g.game_info.home == "BOS") {
                    acc.gf += g.final_result.home;
                    acc.ga += g.final_result.away;
                    acc.shots_per_game += std::accumulate(g.shots.begin(), g.shots.end(), 0.0f, [](auto acc, auto period) { return acc + period.home; });
                    acc.shots_against_per_game += std::accumulate(g.shots.begin(), g.shots.end(), 0.0f, [](auto acc, auto period) { return acc + period.away; });
                    pp.goals += g.power_play.home.goals;
                    pp.attempts += g.power_play.home.attempts;
                    pk.goals += g.power_play.away.goals;
                    pk.attempts += g.power_play.away.attempts;

                } else {
                    acc.gf += g.final_result.away;
                    acc.ga += g.final_result.home;
                    acc.shots_per_game += std::accumulate(g.shots.begin(), g.shots.end(), 0.0f, [](auto acc, auto period) { return acc + period.away; });
                    acc.shots_against_per_game += std::accumulate(g.shots.begin(), g.shots.end(), 0.0f, [](auto acc, auto period) { return acc + period.home; });
                    pp.goals += g.power_play.away.goals;
                    pp.attempts += g.power_play.away.attempts;
                    pk.goals += g.power_play.home.goals;
                    pk.attempts += g.power_play.home.attempts;
                }

                if (g.winning_team == "BOS") {
                    if (auto last_goal_period = g.goals.back().time.period; last_goal_period < 4) {
                        acc.regular_wins++;
                    } else {
                        if (last_goal_period == 4) {
                            acc.ot_wins++;
                        } else if (last_goal_period == 5) {
                            acc.so_wins++;
                            acc.gf--;// shoot-out goals don't end up in the GF stats, nor the GA stats
                        }
                    }
                    acc.won++;
                    acc.points += 2;
                } else {
                    if (auto last_goal_period = g.goals.back().time.period; last_goal_period < 4) {
                        acc.loss++;
                    } else {
                        if (last_goal_period == 5) {
                            acc.ga--;
                        }
                        acc.ot_loss++;
                        acc.points += 1;
                    }
                }
            }
            acc.points_pct = round_to_decimal(acc.points / (games_played * 2), 3);
            acc.pp = round_to_decimal((float(pp.goals) / float(pp.attempts)) * 100.0f, 1);
            auto tmp = (round_to_decimal((1.0 - float(pk.goals) / float(pk.attempts)) * 100.0f, 1));
            acc.pk = tmp;
            acc.gf_avg = round_to_decimal(acc.gf / games_played, 2);
            acc.ga_avg = round_to_decimal(acc.ga / games_played, 2);
            acc.shots_against_per_game = round_to_decimal(acc.shots_against_per_game / games_played, 1);
            acc.shots_per_game = round_to_decimal(acc.shots_per_game / games_played, 1);
        }
        println("Won: {}/{}\nLoss: {}/{}\nOT LOSS: {}/{}\nPoints: {}/{}\nPoints pct: {}/{}\nRegular wins: {}/{}\nOT Wins: {}/{}\nSO Wins: {}/{}\nGF: {}/{}\nGA: {}/{}\nGFA: {}/{}\nGAA: {}/{}\nPP: {}/{}\nPK: {}/{}\nShots: {}/{}\nShots against: {}/{}\n",
                acc.won, stat.won,
                acc.loss, stat.loss,
                acc.ot_loss, stat.ot_loss,
                acc.points, stat.points,
                acc.points_pct, stat.points_pct,
                acc.regular_wins, stat.regular_wins,
                acc.ot_wins, stat.ot_wins,
                acc.so_wins, stat.so_wins,
                acc.gf, stat.gf,
                acc.ga, stat.ga,
                acc.gf_avg, stat.gf_avg,
                acc.ga_avg, stat.ga_avg,
                acc.pp, stat.pp,
                acc.pk, stat.pk,
                acc.shots_per_game, stat.shots_per_game,
                acc.shots_against_per_game, stat.shots_against_per_game);
    }
}

void test_get_games_feb08(Database &db) {
    auto date = CalendarDate{.year = 2021, .month = 2, .day = 8};
    println("Getting games for {}", date.to_string());
    auto _games = db.get_games_at(date);
    auto games = _games.value();
    println("Games {}: {}", date.to_string(), games.size());
    for (auto &g : games) {
        println("{}", g.to_string());
    }
    assert(games.size() == 6);
}

void test_span_average_gf(Database &db) {

    auto all_games = db.get_games_played_by("TORONTO MAPLE LEAFS");
    auto gfs = trend::goals_for("TOR", all_games.value(), 5);
    auto gas = trend::goals_against("TOR", all_games.value(), 5);

    auto pps = trend::power_play("TOR", all_games.value(), 5);
    auto pks = trend::penalty_kill("TOR", all_games.value(), 5);

    // auto pks = span_avg::goals_against("TOR", all_games.value(), 10);
    println("GF 5-span average:");
    for (auto gf : gfs) {
        fmt::print("{}, ", gf.value());
    }
    println("\nGA 5-span average:");
    for (auto ga : gas) {
        fmt::print("{}, ", ga.value());
    }

    println("\nPP efficiency over 5-game span rolling");
    for (auto pp : pps) {
        fmt::print("{}, ", pp.value());
    }

    println("\nPK efficiency over 5-game span rolling");
    for (auto pk : pks) {
        fmt::print("{}, ", pk.value());
    }
}

void test_count_empty_net_goals(Database &db) {
    auto all_games = db.get_all_played_games();
    println("Played games in season: {}", all_games.size());
    auto times_goalie_pulled = 0;
    auto empty_net_goals = 0;
    std::vector<int> games_with_pulled_goalies;
    std::vector<int> games_with_empty_net_goals;

    auto last_game_with_en = 0;
    Goal last_en_goal;
    for (auto &[id, game] : all_games) {
        for (const auto &g : game.goals) {
            if (empty_net_goal(g)) {
                if (id == last_game_with_en) {
                    println("THIS GAME: {} HAD TWO EN GOALS: {} and {}", id, last_en_goal.goal_number, g.goal_number);
                }
                last_en_goal = g;
                last_game_with_en = id;
                games_with_empty_net_goals.push_back(id);
                empty_net_goals++;
            }
        }
        if (game.goalie_probably_pulled) {
            games_with_pulled_goalies.push_back(id);
            times_goalie_pulled++;
        }
    }
    std::ranges::sort(games_with_pulled_goalies);
    std::ranges::sort(games_with_empty_net_goals);

    std::vector<int> pg_dup;
    auto prev = 0;
    for (auto pg : games_with_pulled_goalies) {
        if (prev == pg) pg_dup.push_back(pg);
        prev = pg;
    }
    std::vector<int> en_dup;
    prev = 0;
    for (auto en : games_with_empty_net_goals) {
        if (prev == en) en_dup.push_back(en);
        prev = en;
    }
    println("Duplicates: {} / {}", pg_dup.size(), en_dup.size());
    for (auto i : pg_dup) {
        fmt::print("{}, ", i);
    }
    println("");
    for (auto i : en_dup) {
        fmt::print("{}, ", i);
    }
    println("");
    println("Games with pulled goalies: ");
    auto new_line = 0;
    for (auto pg : games_with_pulled_goalies) {
        fmt::print("{}, ", pg);
        new_line++;
        if (new_line == 10) {
            fmt::print("\n");
            new_line = 0;
        }
    }
    println("\nGames with empty netters: ");
    for (auto en : games_with_empty_net_goals) {
        fmt::print("{}, ", en);
        new_line++;
        if (new_line == 10) {
            fmt::print("\n");
            new_line = 0;
        }
    }
    println("");
    RoundedDecimalNumber<3> empty_net_ratio = static_cast<float>(games_with_empty_net_goals.size()) / static_cast<float>(games_with_pulled_goalies.size()) * 100.0f;
    println("Games with pulled goalie: {} - Games with empty net goals {}. {}%", games_with_pulled_goalies.size(), games_with_empty_net_goals.size(), (float) empty_net_ratio);
}

template <typename Team, typename Games>
auto last_five(const Team& team, const Games& games) {
    GameStatsAccumulator acc{};
    auto begin_at = games.size() - 5;
    auto b = games.begin() + begin_at;
    auto e = games.end();
    for(; b != e; b++) {
        acc.add_game(*b, b->game_info.home == team);
    }
    return acc;
}

void test_win_condition_average(Database &db) {
    auto tor_games = db.get_games_played_by("TOR");
    auto bos_games = db.get_games_played_by("BOS");
    println("Games found by BOS: {}", bos_games->size());

    auto win_averages = [](const auto& team, const auto& games) {
      std::vector<Game> won_games;
      won_games.reserve(games->size());
      std::ranges::copy_if(games.value(), std::back_inserter(won_games), [&](const Game& game) {
        return game.winning_team == team;
      });
      auto win_conditions_avg = total::win_condition_averages(team, won_games);
      return win_conditions_avg;
    };

    auto tor_win_avg = win_averages("TOR", tor_games);
    auto bos_win_avg = win_averages("BOS", bos_games);

    auto tor_last_5 = last_five("TOR", tor_games.value()) / 5.0;
    auto bos_last_5 = last_five("BOS", bos_games.value()) / 5.0;
    assert(tor_last_5.games_accumulated == 5 && bos_last_5.games_accumulated == 5);

    println("\nTORONTO MAPLE LEAFS WIN CONDITIONS");
    tor_win_avg.print();
    println("\nBOSTON BRUINS WIN CONDITIONS");
    bos_win_avg.print();

    auto tor_diff = tor_win_avg.difference(bos_win_avg);
    auto bos_diff = bos_win_avg.difference(tor_win_avg);
    println("\nTOR VS BOS");
    tor_diff.print();
    println("\nBOS VS TOR");
    bos_diff.print();

    print_comparison({"TOR", tor_last_5}, {"BOS", bos_last_5});
    print_comparison( {"BOS", bos_last_5}, {"TOR", tor_last_5});
}

void calculate_games_reaching_OT(Database& db) {
    const auto all_games = db.get_all_played_games();
    std::vector<Game> games{};
    games.reserve(all_games.size());
    std::ranges::transform(all_games, std::back_inserter(games), [](const auto& kvp) {
       return kvp.second;
    });

    auto res = trend::overtime_games_percentage(games, games.size());
    println("Games reaching OT: {:1f}%", res[0].as_percent());

}

int main(int argc, const char **argv) {
    auto db = Database::create_and_setup("assets");
    // verify_toronto_team_standings(*db);
    // test_win_condition_average(*db);
    calculate_games_reaching_OT(*db);
}
