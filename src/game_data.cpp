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

float SpecialTeams::get_efficiency(SpecialTeams::Type type) const {
    switch(type) {
        case Type::PowerPlay:
            return float(goals) / float(attempts);
        case Type::PenaltyKilling:
            return 1.0f - float(goals) / float(attempts);
    }
}
