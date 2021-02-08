//
// Created by 46769 on 2021-02-07.
//

#include "../include/statsdb/db.hpp"
#include "deserialize.hpp"
#include <fmt/core.h>
#include <fstream>
#include <ranges>
#include <string_view>

std::optional<std::string> file_string(const fs::path &file, int reservedSpace) {
    if (fs::exists(file)) {
        std::string buf{};
        std::ifstream f{file};
        buf.reserve(reservedSpace);
        buf.assign(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
        return buf;
    } else {
        return {};
    }
}

std::vector<Team> deserialize_teams(const fs::path &teams_file) {
    if (fs::exists(teams_file)) {
        auto buf = file_string(teams_file, kilobytes(3));
        if (buf) {
            return json::parse(buf.value());
        } else {
            return {};
        }
    } else {
        return {};
    }
}
std::vector<GameInfo> deserialize_game_infos(const fs::path &games_info_file) {
    if (fs::exists(games_info_file)) {
        auto buf = file_string(games_info_file, kilobytes(75));
        if (buf) {
            return json::parse(buf.value());
        } else {
            return {};
        }
    } else {
        return {};
    }
}

std::vector<Game> deserialize_games(const fs::path &games_file) {
    if (fs::exists(games_file)) {
        auto buf = file_string(games_file, kilobytes(350));
        if (buf) {
            return json::parse(buf.value());
        } else {
            return {};
        }
    } else {
        return {};
    }
}

Database::DBHandle Database::create(const fs::path &assets_directory) {
    if (fs::exists(assets_directory)) {
        auto teams_data = deserialize_teams(assets_directory / "db" / "teams.db");
        auto game_infos_data = deserialize_game_infos(assets_directory / "db" / "gameinfo.db");
        auto games_data = deserialize_games(assets_directory / "db" / "gameresults.db");
        if (teams_data.empty() || game_infos_data.empty() || games_data.empty()) {
            println("Deserializing data failed\n. Teams {} - Game Infos {} - Games {}", teams_data.size(), game_infos_data.size(), games_data.size());
            std::abort();
        }

        std::map<int, Team> teams{};
        std::map<int, GameInfo> gameInfos{};
        std::map<int, Game> gameResults{};

        for (const auto &t : teams_data) {
            teams.emplace(t.id, t);
        }

        for (const auto &gi : game_infos_data) {
            gameInfos.emplace(gi.game_id, gi);
        }

        for (const auto &gr : games_data) {
            gameResults.emplace(gr.game_info.game_id, gr);
        }
        return std::make_unique<Database>(std::move(teams), std::move(gameInfos), std::move(gameResults));

    } else {

        println("Assets directory {} does not exist.\n", (fs::current_path() / assets_directory).string());
        std::abort();
    }
}
Database::Database(Map<Team> &&team, Map<GameInfo> &&schedule, Map<Game> &&games_played) noexcept : teams(std::move(team)), schedule(std::move(schedule)), played_games(std::move(games_played)), calendar{}, games_by_team{} {
    init();
}
void Database::init() {
    for (const auto &[id, game_info] : schedule) {
        auto it = std::find_if(calendar.begin(), calendar.end(), [date = game_info.date](const auto &it) {
            return it.first == date;
        });
        if (it != calendar.end()) {
            calendar[game_info.date].insert(game_info.game_id);
        } else {
            std::set<int> new_set{};
            new_set.insert(game_info.game_id);
            calendar.emplace(game_info.date, std::move(new_set));
        }
    }


    // Not a code smell. We are creating a map of references to data we own. Must therefore be done here.
    for (const auto &[game_id, game_result] : played_games) {
        const auto &home_team = game_result.game_info.home;
        const auto &away_team = game_result.game_info.away;
        auto home_search = std::find_if(teams.cbegin(), teams.cend(), [&](const auto &iter) {
            const auto &team = iter.second;
            if (team == home_team) {
                return true;
            }
            return false;
        });
        auto away_search = std::find_if(teams.cbegin(), teams.cend(), [&](const auto &iter) {
            const auto &team = iter.second;
            if (team == away_team) {
                return true;
            }
            return false;
        });

        if (home_search != teams.end() && away_search != teams.end()) {
            auto ht = home_search->second;
            auto at = away_search->second;
            games_by_team[ht].insert(&game_result);
            games_by_team[at].insert(&game_result);
        } else {
            println("failed to initialize DB.");
            std::abort();
        }
    }
    for (auto &t : games_by_team) {
        println("{} has played {} games", t.first.name, t.second.size());
    }
}
std::optional<std::vector<GameInfo>> Database::get_games_at(const CalendarDate &date) {
    if (calendar.find(date) != calendar.end()) {
        auto game_ids = calendar[date];
        std::vector<GameInfo> games;
        std::transform(game_ids.cbegin(), game_ids.cend(), std::back_inserter(games), [&](const auto &id) {
            return schedule[id];
        });
        return games;
    }
    return {};
}
std::optional<GameInfo> Database::get_game_info(int game_id) {
    if (schedule.find(game_id) != schedule.end()) {
        return schedule[game_id];
    } else {
        return {};
    }
}
std::optional<Game> Database::get_game(int game_id) {
    if (auto item = std::ranges::find_if(played_games, [id = game_id](auto g) { return id == g.second.game_info.game_id; }); item != std::end(played_games)) {
        return item->second;
    } else {
        return {};
    }
}
std::optional<std::vector<Game>> Database::get_games_played_by(const std::string &teamName) {
    auto search_term = teamName;
    std::ranges::transform(search_term, search_term.begin(), [](auto ch) { return std::toupper(ch); });
    println("Searching for team {}", search_term);
    auto team_search = std::find_if(teams.cbegin(), teams.cend(), [&](const auto &iter) {
        const auto &team = iter.second;
        if (team == search_term) {
            return true;
        }
        return false;
    });
    if (team_search != teams.end()) {
        std::vector<Game> games{};
        // regular season, tend to be max 81 games
        games.reserve(81);
        auto team_name = team_search->second;
        auto games_refs = games_by_team[team_name];
        std::ranges::transform(games_refs, std::back_inserter(games), [](auto el) {
            return *el;
        });
        return games;
    } else {
        return {};
    }
}
