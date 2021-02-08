//
// Created by Simon Farre on 2021-02-07.
//

#pragma once
#include <nlohmann/json.hpp>
#include "../include/statsdb/game_data.hpp"
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

using json = nlohmann::json;

constexpr auto kilobytes(int amount) { return 1024 * amount; }
constexpr auto megabytes(int amount) { return 1024 * kilobytes(amount); }

void from_json(const json& j, Team& t);
void from_json(const json& j, CalendarDate& d);
void from_json(const json& j, GameInfo& gi);
void from_json(const json& j, Goal& goal);
void from_json(const json& j, IntResult& value_pair);
void from_json(const json& j, Game& game);

[[nodiscard]] std::optional<std::string> file_string(const fs::path& file, int reserve_space = 0);