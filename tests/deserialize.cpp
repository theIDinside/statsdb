//
// Created by 46769 on 2021-02-07.
//
#include <cassert>
#include <iostream>
#include <numeric>
#include <statsdb/statsdb.h>


template<typename T>
float round_to_decimal(T value, int decimal_points) {
    auto v = 792.0 / 1000.0;
    float res_ = 756.0 / 1000.0;
    double pow = std::pow(10, decimal_points);
    double tmp = std::round(value * pow);
    auto res = double(tmp) / pow;

    return res;
}
/*
template <int DecimalPoints>
constexpr float round_to(float value) {
    static_assert(DecimalPoints < 7);
    if constexpr (DecimalPoints == 1) {
        value += 0.05;
    } else if constexpr(DecimalPoints == 2) {

    }
    else if constexpr(DecimalPoints == 3) {

    }
    else if constexpr(DecimalPoints == 4) {

    }
    else if constexpr(DecimalPoints == 5) {

    }
    else if constexpr(DecimalPoints == 6) {

    }
}
*/
static constexpr CalendarDate TestDate{.year = 2021, .month = 02, .day = 7};

void test_get_toronto_played(Database &db) {
    auto games = db.get_games_played_by("TOR");
    if (games) {
        std::cout << "FOUND " << games.value().size() << " games played " << std::endl;
    }
}

void test_get_toronto_wins(Database &db) {
    auto games = db.get_games_played_by("TORONTO MAPLE LEAFS");
    games->erase(std::remove_if(games->begin(), games->end(), [](auto g) {
                     return g.game_info.date > TestDate;
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
                     return g.game_info.date > TestDate;
                 }),
                 games->end());
    if (games) {
        auto gs = games.value();
        auto acc_gf = 0.0;
        auto acc_ga = 0.0;
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

void test_toronto_team_standings_of_8th_of_feb(Database &db) {
    struct Stats {
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
    } acc{};

    Stats teamStanding{
            .won = 9,   //
            .loss = 2, //
            .ot_loss = 1, //
            .points = 19, //
            .points_pct = 0.792,
            .regular_wins = 8, //
            .ot_wins = 1, //
            .so_wins = 0, //
            .gf = 45,
            .ga = 33,
            .gf_avg = 3.75, //
            .ga_avg = 2.75, //
            .pp = 38.5,
            .pk = 75.6,
            .shots_per_game = 30.6,
            .shots_against_per_game = 26.9};
    
    auto games = db.get_games_played_by("TORONTO MAPLE LEAFS");
    games->erase(std::remove_if(games->begin(), games->end(), [](auto g) {
                     return g.game_info.date > TestDate;
                 }),
                 games->end());
    if (games) {
        auto gs = games.value();
        auto acc_gf = 0.0;
        auto acc_ga = 0.0;
        SpecialTeams pp{};
        SpecialTeams pk{};
        auto games_played = static_cast<double>(gs.size());
        for (auto &g : gs) {
            if (g.game_info.home == "TOR") {
                acc.gf += g.final_result.home;
                acc.ga += g.final_result.away;
                acc.shots_per_game += std::accumulate(g.shots.begin(), g.shots.end(), 0.0, [](auto acc, auto period) { return acc + period.home; });
                acc.shots_against_per_game += std::accumulate(g.shots.begin(), g.shots.end(), 0.0, [](auto acc, auto period) { return acc + period.away; });
                pp.goals += g.power_play.home.goals;
                pp.attempts += g.power_play.home.attempts;
                pk.goals += g.power_play.away.goals;
                pk.attempts += g.power_play.away.attempts;

            } else {
                acc.gf += g.final_result.away;
                acc.ga += g.final_result.home;
                acc.shots_per_game += std::accumulate(g.shots.begin(), g.shots.end(), 0.0, [](auto acc, auto period) { return acc + period.away; });
                acc.shots_against_per_game += std::accumulate(g.shots.begin(), g.shots.end(), 0.0, [](auto acc, auto period) { return acc + period.home; });
                pp.goals += g.power_play.away.goals;
                pp.attempts += g.power_play.away.attempts;
                pk.goals += g.power_play.home.goals;
                pk.attempts += g.power_play.home.attempts;
            }

            if(g.winning_team == "TOR") {
                if(auto last_goal_period = g.goals.back().time.period; last_goal_period < 4) {
                    acc.regular_wins++;
                } else {
                    if(last_goal_period == 4) {
                        acc.ot_wins++;
                    } else if(last_goal_period == 5) {
                        acc.so_wins++;
                    }
                }
                acc.won++;
                acc.points += 2;
            } else {
                if(auto last_goal_period = g.goals.back().time.period; last_goal_period < 4) {
                    acc.loss++;
                } else {
                    acc.ot_loss++;
                    acc.points += 1;
                }
            }
        }
        acc.points_pct = round_to_decimal(acc.points / (games_played * 2), 3);
        acc.pp = round_to_decimal((float(pp.goals) / float(pp.attempts)) * 100.0, 1);
        auto tmp = (1.0 - round_to_decimal(float(pk.goals) / float(pk.attempts), 3)) * 100;
        acc.pk = tmp;
        acc.gf_avg = round_to_decimal(acc.gf / games_played, 2);
        acc.ga_avg = round_to_decimal(acc.ga / games_played, 2);
        acc.shots_against_per_game = round_to_decimal(acc.shots_against_per_game / games_played, 1);
        acc.shots_per_game = round_to_decimal(acc.shots_per_game / games_played, 1);

    }
    println("Won: {}/{}\nLoss: {}/{}\nOT LOSS: {}/{}\nPoints: {}/{}\nPoints pct: {}/{}\nRegular wins: {}/{}\nOT Wins: {}/{}\nSO Wins: {}/{}\nGF: {}/{}\nGA: {}/{}\nGFA: {}/{}\nGAA: {}/{}\nPP: {}/{}\nPK: {}/{}\nShots: {}/{}\nShots against: {}/{}\n",
    acc.won,                    teamStanding.won,
    acc.loss,                   teamStanding.loss,
    acc.ot_loss,                teamStanding.ot_loss,
    acc.points,                 teamStanding.points,
    acc.points_pct,             teamStanding.points_pct,
    acc.regular_wins,           teamStanding.regular_wins,
    acc.ot_wins,                teamStanding.ot_wins,
    acc.so_wins,                teamStanding.so_wins,
    acc.gf,                     teamStanding.gf,
    acc.ga,                     teamStanding.ga,
    acc.gf_avg,                 teamStanding.gf_avg,
    acc.ga_avg,                 teamStanding.ga_avg,
    acc.pp,                     teamStanding.pp,
    acc.pk,                     teamStanding.pk,
    acc.shots_per_game,         teamStanding.shots_per_game,
    acc.shots_against_per_game, teamStanding.shots_against_per_game);
    println("Assertion: {} == {}", acc.pk, 75.6);
    assert(acc.pk == 75.6f);
}

void test_get_games_feb08(Database& db) {
    auto date = CalendarDate{.year = 2021, .month = 2, .day = 8};
    println("Getting games for {}", date.to_string());
    auto _games = db.get_games_at(date);
    auto games = _games.value();
    println("Games {}: {}", date.to_string(), games.size());
    for(auto& g : games) {
        println("{}", g.to_string());
    }
    assert(games.size() == 6);
}

int main(int argc, const char **argv) {
    std::cout << "assets root dir set at: "
              << "./assets" << std::endl;
    auto db = Database::create("assets");
    test_get_toronto_played(*db);
    test_get_toronto_wins(*db);
    test_toronto_goals(*db);
    test_toronto_team_standings_of_8th_of_feb(*db);
    test_get_games_feb08(*db);
}