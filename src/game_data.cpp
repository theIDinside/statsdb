#include "../include/statsdb/game_data.hpp"
#include <algorithm>
bool operator==(const Team& lhs, const Team& rhs) {
    return lhs.id == rhs.id;
}

bool operator==(const Team &lhs, std::string_view team_string) {
    if(team_string == lhs.name) {
        return true;
    }
    return std::ranges::any_of(lhs.abbreviations, [&](auto abbr) {
      return abbr == team_string;
    });
}

bool operator==(const Team& lhs, const std::string& team_string) {
    if(team_string == lhs.name) {
        return true;
    }
    return std::ranges::any_of(lhs.abbreviations, [&](auto abbr) {
      return abbr == team_string;
    });
}

bool operator<(const Team& lhs, const Team& rhs) {
    return lhs.id < rhs.id;
}
bool operator>(const Team& lhs, const Team& rhs) {
    return lhs.id > rhs.id;
}

