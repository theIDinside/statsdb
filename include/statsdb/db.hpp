//
// Created by 46769 on 2021-02-07.
//

#pragma once
#include "game_data.hpp"

namespace fs = std::filesystem;

/// std::map, where keys are ID's, either Team ID's or Game ID's
template<typename T>
using IDMap = std::map<int, T>;
using Schedule = std::map<CalendarDate, std::set<int>>;
using TeamGamesMap = std::map<Team, std::set<const Game *>>;

class Database { /* <'a> */
public:
    Database(IDMap<Team> &&team, IDMap<GameInfo> &&schedule, IDMap<Game> &&games_played, Schedule&& calendar, TeamGamesMap&& games_by_team) noexcept;
    using DBHandle = std::unique_ptr<Database>;
    static DBHandle create_and_setup(const fs::path &assets_directory);
    std::optional<std::vector<GameInfo>> get_games_at(const CalendarDate &date);
    std::optional<GameInfo> get_game_info(u32 game_id);

    std::optional<Game> get_game(u32 game_id);
    std::optional<std::vector<Game>> get_games_played_by(const std::string &teamName);
    [[nodiscard]] const IDMap<Game>& get_all_played_games() const;

private:
    const IDMap<Team> teams;
    const IDMap<GameInfo> schedule;
    const IDMap<Game> played_games;
    /// Data that refers to other data, within this object
    const Schedule calendar;                                   // <'a>
    const TeamGamesMap games_by_team;// <'a>
};
