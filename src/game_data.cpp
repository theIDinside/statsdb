#include "../include/statsdb/game_data.hpp"
#include <numeric>

template<typename Res>
constexpr Res accumulate_as(auto container, auto fn) {
    return std::accumulate(container.begin(), container.end(), Res{}, fn);
}

bool operator==(const Team &lhs, const Team &rhs) {
    return lhs.id == rhs.id;
}

bool operator==(const Team &lhs, std::string_view team_string) {
    if (team_string == lhs.name) {
        return true;
    }
    return std::ranges::any_of(lhs.abbreviations, [&](auto abbr) {
        return abbr == team_string;
    });
}

bool operator==(const Team &lhs, const std::string &team_string) {
    if (team_string == lhs.name) {
        return true;
    }
    return std::ranges::any_of(lhs.abbreviations, [&](auto abbr) {
        return abbr == team_string;
    });
}

bool operator<(const Team &lhs, const Team &rhs) {
    return lhs.id < rhs.id;
}
bool operator>(const Team &lhs, const Team &rhs) {
    return lhs.id > rhs.id;
}

bool empty_net_goal(const Goal &g) {
    switch (g.strength) {
        case TeamStrength::EVEN_EMPTY_NET:
        case TeamStrength::SHORT_HANDED_EMPTY_NET:
        case TeamStrength::POWER_PLAY_EMPTY_NET:
            return true;
        default:
            return false;
    }
}

SpecialTeams operator+(const SpecialTeams &left, const SpecialTeams& right) {
    return {
        .goals = left.goals + right.goals,
        .attempts = left.attempts + right.attempts
    };
}

SpecialTeams get_special_teams(const Game &game, std::string_view team, SpecialTeams::Type type) {
    assert(game.game_info.home == team || game.game_info.away == team);
    if (game.game_info.home == team) {
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

StatsAverage operator/(const StatsAverage &acc, float divisor) {
    auto res = acc;
    res.pp_efficiency.home /= divisor;
    res.pp_efficiency.away /= divisor;
    res.pk_efficiency.home /= divisor;
    res.pk_efficiency.away /= divisor;
    res.goals.home /= divisor;
    res.goals.away /= divisor;
    res.shots.home /= divisor;
    res.shots.away /= divisor;
    res.blocked_shots.home /= divisor;
    res.blocked_shots.away /= divisor;
    res.face_off.home /= divisor;
    res.face_off.away /= divisor;
    res.give_aways.home /= divisor;
    res.give_aways.away /= divisor;
    res.take_aways.home /= divisor;
    res.take_aways.away /= divisor;
    res.hits.home /= divisor;
    res.hits.away /= divisor;
    res.pim.home /= divisor;
    res.pim.away /= divisor;
    res.pp.home /= divisor;
    res.pp.away /= divisor;
    res.pk.home /= divisor;
    res.pk.away /= divisor;
    return res;
}
void StatsAverage::add_game(const Game &g, bool isHomeTeam) {
    if (isHomeTeam) {
        pp_efficiency.home += g.power_play.home.get_efficiency<NumberFormat::Percent>(SpecialTeams::Type::PowerPlay);
        pp_efficiency.away += g.power_play.away.get_efficiency<NumberFormat::Percent>(SpecialTeams::Type::PowerPlay);
        pk_efficiency.home += g.power_play.away.get_efficiency<NumberFormat::Percent>(SpecialTeams::Type::PenaltyKilling);
        pk_efficiency.away += g.power_play.home.get_efficiency<NumberFormat::Percent>(SpecialTeams::Type::PenaltyKilling);

        goals.home += g.final_result.home;
        goals.away += g.final_result.away;
        shots.home += accumulate_as<float>(g.shots, [](auto acc, auto p) {
            return acc + p.home;
        });
        shots.away += accumulate_as<float>(g.shots, [](auto acc, auto p) {
            return acc + p.away;
        });
        blocked_shots.home += g.blocked_shots.home;
        blocked_shots.away += g.blocked_shots.away;
        face_off.home += g.face_off.home;
        face_off.away += g.face_off.away;
        give_aways.home += g.give_aways.home;
        give_aways.away += g.give_aways.away;
        take_aways.home += g.take_aways.home;
        take_aways.away += g.take_aways.away;
        hits.home += g.hits.home;
        hits.away += g.hits.away;
        pim.home += g.pim.home;
        pim.away += g.pim.away;
        pp.home += g.power_play.home.attempts;
        pk.home += g.power_play.away.attempts;
        pp.away += g.power_play.away.attempts;
        pk.away += g.power_play.home.attempts;
    } else {
        pp_efficiency.home += g.power_play.away.get_efficiency<NumberFormat::Percent>(SpecialTeams::Type::PowerPlay);
        pp_efficiency.away += g.power_play.home.get_efficiency<NumberFormat::Percent>(SpecialTeams::Type::PowerPlay);
        pk_efficiency.home += g.power_play.away.get_efficiency<NumberFormat::Percent>(SpecialTeams::Type::PenaltyKilling);
        pk_efficiency.away += g.power_play.home.get_efficiency<NumberFormat::Percent>(SpecialTeams::Type::PenaltyKilling);

        goals.home += g.final_result.away;
        goals.away += g.final_result.home;
        shots.home += accumulate_as<float>(g.shots, [](auto acc, auto p) {
            return acc + p.away;
        });
        shots.away += accumulate_as<float>(g.shots, [](auto acc, auto p) {
            return acc + p.home;
        });
        blocked_shots.home += g.blocked_shots.away;
        blocked_shots.away += g.blocked_shots.home;
        face_off.home += g.face_off.away;
        face_off.away += g.face_off.home;
        give_aways.home += g.give_aways.away;
        give_aways.away += g.give_aways.home;
        take_aways.home += g.take_aways.away;
        take_aways.away += g.take_aways.home;
        hits.home += g.hits.away;
        hits.away += g.hits.home;
        pim.home += g.pim.away;
        pim.away += g.pim.home;
        pp.home += g.power_play.away.attempts;
        pk.home += g.power_play.home.attempts;
        pp.away += g.power_play.home.attempts;
        pk.away += g.power_play.away.attempts;
    }
}

void StatsAverage::print() const {
    using NumFmt = RoundedDecimalNumber<2>;
    println("{:-^68}", "Win conditions average");
    auto header = fmt::format("{:-^15}|{:->50}", "Stats", "----- Team average ----- | ----- Opp. average -----|");
    auto goals_ = fmt::format("{:<15}|{:^25}|{:^25}|", "Goals", NumFmt{goals.home}.value(), NumFmt{goals.away}.value());
    auto shots_ = fmt::format("{:<15}|{:^25}|{:^25}|", "Shots", NumFmt{shots.home}.value(), NumFmt{shots.away}.value());
    auto pp_efficiency_ = fmt::format("{:<15}|{:^25}|{:^25}|", "PP", fmt::format("{}%", NumFmt{pp_efficiency.home}.value()), fmt::format("{}%", NumFmt{pp_efficiency.away}.value()));
    auto pk_efficiency_ = fmt::format("{:<15}|{:^25}|{:^25}|", "PK", fmt::format("{}%", NumFmt{pk_efficiency.home}.value()), fmt::format("{}%", NumFmt{pk_efficiency.away}.value()));
    auto pp_ = fmt::format("{:<15}|{:^25}|{:^25}|", "PP/game", NumFmt{pp.home}.value(), NumFmt{pp.away}.value());
    auto pk_ = fmt::format("{:<15}|{:^25}|{:^25}|", "PK/game", NumFmt{pk.home}.value(), NumFmt{pk.away}.value());
    auto pim_ = fmt::format("{:<15}|{:^25}|{:^25}|", "PIM", NumFmt{pim.home}.value(), NumFmt{pim.away}.value());
    auto hits_ = fmt::format("{:<15}|{:^25}|{:^25}|", "Hits", NumFmt{hits.home}.value(), NumFmt{hits.away}.value());
    auto face_off_ = fmt::format("{:<15}|{:^25}|{:^25}|", "Faceoff", fmt::format("{}%", NumFmt{face_off.home}.value()), fmt::format("{}%", NumFmt{face_off.away}.value()));
    auto blocked_ = fmt::format("{:<15}|{:^25}|{:^25}|", "Blocked shots", NumFmt{blocked_shots.home}.value(), NumFmt{blocked_shots.away}.value());
    auto give_aways_ = fmt::format("{:<15}|{:^25}|{:^25}|", "Giveaways", NumFmt{give_aways.home}.value(), NumFmt{give_aways.away}.value());
    auto take_aways_ = fmt::format("{:<15}|{:^25}|{:^25}|", "Takeaways", NumFmt{take_aways.home}.value(), NumFmt{take_aways.away}.value());
    println(
            "{}\n{}\n{}\n{}\n{}\n{}\n{}\n{}\n{}\n{}\n{}\n{}\n{}",
            header,
            goals_,
            shots_,
            pp_efficiency_,
            pk_efficiency_,
            pp_,
            pk_,
            pim_,
            hits_,
            face_off_,
            blocked_,
            give_aways_,
            take_aways_);
}
StatsAverage StatsAverage::difference(const StatsAverage &other) {
    return StatsAverage{
            .goals = goals.difference(other.goals),
            .shots = shots.difference(other.shots),
            .pp_efficiency = pp_efficiency.difference(other.pp_efficiency),
            .pk_efficiency = pk_efficiency.difference(other.pk_efficiency),
            .pim = pim.difference(other.pim),
            .hits = hits.difference(other.hits),
            .face_off = face_off.difference(other.face_off),
            .blocked_shots = blocked_shots.difference(other.blocked_shots),
            .give_aways = give_aways.difference(other.give_aways),
            .take_aways = take_aways.difference(other.take_aways)};
}

void GameStatsAccumulator::add_game(const Game &g, bool isHomeTeam) {
    if (isHomeTeam) {
        games_accumulated++;
        goals += g.final_result.home;
        shots += accumulate_as<float>(g.shots, [](auto acc, auto period) { return acc + period.home; });
        pp = pp + g.power_play.home;
        pk = pk + g.power_play.away;
        pim += 0;
        hits += 0;
        face_off += g.face_off.home;
        blocked_shots += g.blocked_shots.home;
        give_aways += g.give_aways.home;
        take_aways += g.take_aways.home;
    } else {
        games_accumulated++;
        goals += g.final_result.away;
        shots += accumulate_as<float>(g.shots, [](auto acc, auto period) { return acc + period.away; });
        pp = pp + g.power_play.away;
        pk = pk + g.power_play.home;
        pim += 0;
        hits += 0;
        face_off += g.face_off.away;
        blocked_shots += g.blocked_shots.away;
        give_aways += g.give_aways.away;
        take_aways += g.take_aways.away;
    }
}
GameStatsAccumulator operator/(const GameStatsAccumulator &acc, float divisor) {
    auto res = acc;
    return GameStatsAccumulator {
            .games_accumulated = acc.games_accumulated,
            .goals = acc.goals / divisor,
            .shots = acc.shots / divisor,
            .pp = acc.pp,
            .pk = acc.pk,
            .pim = acc.pim / divisor,
            .hits = acc.hits / divisor,
            .face_off = acc.face_off / divisor,
            .blocked_shots = acc.blocked_shots / divisor,
            .give_aways = acc.give_aways / divisor,
            .take_aways = acc.take_aways / divisor
    };
}

void print_comparison(const AverageInfo &home, const AverageInfo &away) {
    constexpr auto EffFmt = NumberFormat::Percent;
    using NumFmt = RoundedDecimalNumber<2>;
    auto top = fmt::format(" Avg comparison - {} {} games played - {} {} games played ", home.team_name, home.data.games_accumulated, away.team_name, away.data.games_accumulated);
    println("{:-^68}", top);
    auto teams = fmt::format("{:-^25}|{:-^25}", home.team_name, away.team_name);
    auto header = fmt::format("{:-^15}|{:->50}", "Stats", teams);
    auto goals_ = fmt::format("{:<15}|{:^25}|{:^25}|", "Goals", NumFmt{home.data.goals}.value(), NumFmt{away.data.goals}.value());
    auto shots_ = fmt::format("{:<15}|{:^25}|{:^25}|", "Shots", NumFmt{home.data.shots}.value(), NumFmt{away.data.shots}.value());
    auto pp_efficiency_ = fmt::format("{:<15}|{:^25}|{:^25}|", "PP", fmt::format("{}%", NumFmt{home.data.pp.get_efficiency<EffFmt>(SpecialTeams::Type::PowerPlay)}.value()), fmt::format("{}%", NumFmt{away.data.pp.get_efficiency<EffFmt>(SpecialTeams::Type::PowerPlay)}.value()));
    auto pk_efficiency_ = fmt::format("{:<15}|{:^25}|{:^25}|", "PK", fmt::format("{}%", NumFmt{home.data.pk.get_efficiency<EffFmt>(SpecialTeams::Type::PenaltyKilling)}.value()), fmt::format("{}%", NumFmt{away.data.pk.get_efficiency<EffFmt>(SpecialTeams::Type::PenaltyKilling)}.value()));
    auto pp_ = fmt::format("{:<15}|{:^25}|{:^25}|", "PP/game", NumFmt{float(home.data.pp.attempts) / float(home.data.games_accumulated)}.value(), NumFmt{float(away.data.pp.attempts) / float(away.data.games_accumulated)}.value());
    auto pk_ = fmt::format("{:<15}|{:^25}|{:^25}|", "PK/game", NumFmt{float(home.data.pk.attempts) / float(home.data.games_accumulated) }.value(), NumFmt{float(away.data.pk.attempts) / float(away.data.games_accumulated)}.value());
    auto pim_ = fmt::format("{:<15}|{:^25}|{:^25}|", "PIM", NumFmt{home.data.pim}.value(), NumFmt{away.data.pim}.value());
    auto hits_ = fmt::format("{:<15}|{:^25}|{:^25}|", "Hits", NumFmt{home.data.hits}.value(), NumFmt{away.data.hits}.value());
    auto face_off_ = fmt::format("{:<15}|{:^25}|{:^25}|", "Faceoff", fmt::format("{}%", NumFmt{home.data.face_off}.value()), fmt::format("{}%", NumFmt{away.data.face_off}.value()));
    auto blocked_ = fmt::format("{:<15}|{:^25}|{:^25}|", "Blocked shots", NumFmt{home.data.blocked_shots}.value(), NumFmt{away.data.blocked_shots}.value());
    auto give_aways_ = fmt::format("{:<15}|{:^25}|{:^25}|", "Giveaways", NumFmt{home.data.give_aways}.value(), NumFmt{away.data.give_aways}.value());
    auto take_aways_ = fmt::format("{:<15}|{:^25}|{:^25}|", "Takeaways", NumFmt{home.data.take_aways}.value(), NumFmt{away.data.take_aways}.value());
    println(
            "{}\n{}\n{}\n{}\n{}\n{}\n{}\n{}\n{}\n{}\n{}\n{}\n{}",
            header,
            goals_,
            shots_,
            pp_efficiency_,
            pk_efficiency_,
            pp_,
            pk_,
            pim_,
            hits_,
            face_off_,
            blocked_,
            give_aways_,
            take_aways_);
}
