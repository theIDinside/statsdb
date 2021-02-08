//
// Created by 46769 on 2021-02-07.
//

#pragma once
#include <filesystem>
#include <map>
#include <optional>
#include <set>

#include "game_data.hpp"

namespace fs = std::filesystem;

/// std::map, where keys are ID's, either Team ID's or Game ID's
template<typename T>
using IDMap = std::map<int, T>;
using Schedule = std::map<CalendarDate, std::set<int>>;

class Database { /* <'a> */
public:
    Database(IDMap<Team> &&team, IDMap<GameInfo> &&schedule, IDMap<Game> &&games_played) noexcept;
    using DBHandle = std::unique_ptr<Database>;
    static DBHandle create(const fs::path &assets_directory);
    std::optional<std::vector<GameInfo>> get_games_at(const CalendarDate &date);
    std::optional<GameInfo> get_game_info(int game_id);

    std::optional<Game> get_game(int game_id);
    std::optional<std::vector<Game>> get_games_played_by(const std::string &teamName);

private:
    IDMap<Team> teams;
    IDMap<GameInfo> schedule;
    IDMap<Game> played_games;

    /// Initializes relative data (calendar, and games_by_team, two members which hold references to data inside this class, so are bounded by it's lifetime 'a
    void init();
    Schedule calendar;                                   // <'a>
    std::map<Team, std::set<const Game *>> games_by_team;// <'a>
};
