#include "../include/statsdb/game_data.hpp"

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
bool empty_net_goal(const Goal &g) {
    switch(g.strength) {
        case TeamStrength::EVEN_EMPTY_NET:
        case TeamStrength::SHORT_HANDED_EMPTY_NET:
        case TeamStrength::POWER_PLAY_EMPTY_NET:
            return true;
        default:
            return false;
    }
}

float SpecialTeams::get_efficiency(SpecialTeams::Type type) const {
    switch(type) {
        case Type::PowerPlay:
            return float(goals) / float(attempts);
        case Type::PenaltyKilling:
            return 1.0f - float(goals) / float(attempts);
    }
}

SpecialTeams get_special_teams(const Game &game, std::string_view team, SpecialTeams::Type type) {
    assert(game.game_info.home == team || game.game_info.away == team);
    if(game.game_info.home == team) {
        switch (type) {
            case SpecialTeams::Type::PowerPlay:
                return game.power_play.home;
            case SpecialTeams::Type::PenaltyKilling:
                return game.power_play.away;
        }
    } else {
        switch (type) {
            case SpecialTeams::Type::PowerPlay:
                return game.power_play.away;
            case SpecialTeams::Type::PenaltyKilling:
                return game.power_play.home;
        }
    }
}