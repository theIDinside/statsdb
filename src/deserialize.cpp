//
// Created by Simon Farre on 2021-02-07.
//
#include "deserialize.hpp"
#include <fstream>


void from_json(const json& j, Team& t) {
    j.at("id").get_to(t.id);
    j.at("name").get_to(t.name);
    j.at("abbreviations").get_to(t.abbreviations);
}

void from_json(const json& j, CalendarDate& d) {
    j.at("day").get_to(d.day);
    j.at("month").get_to(d.month);
    j.at("year").get_to(d.year);
}

void from_json(const json& j, GameInfo& gi) {
    j.at("home").get_to(gi.home);
    j.at("away").get_to(gi.away);
    j.at("date").get_to(gi.date);
    j.at("gid").get_to(gi.game_id);
}

void from_json(const json& j, Goal& goal) {
    // TODO: This is not fully implemented and will create runtime logical errors (strength not read for example)
    j.at("goal_number").get_to(goal.goal_number);
    j.at("player").get_to(goal.scoring_player);
    j.at("team").get_to(goal.scoring_team);

    j.at("period").at("number").get_to(goal.time.period);
    j.at("period").at("time").at("minutes").get_to(goal.time.time.minutes);
    j.at("period").at("time").at("seconds").get_to(goal.time.time.seconds);

    std::string strength = j["strength"];
    if(strength == "Even") {
        goal.strength = TeamStrength::EVEN;
    } else if(strength == "PowerPlay") {
        goal.strength = TeamStrength::POWER_PLAY;
    } else if(strength == "EvenEmptyNet") {
        goal.strength = TeamStrength::EVEN_EMPTY_NET;
    } else if(strength == "Shootout") {
        goal.strength = TeamStrength::SHOOTOUT;
    } else if(strength == "ShortHanded") {
        goal.strength = TeamStrength::SHORT_HANDED;
    } else if(strength == "PowerPlayPenaltyShot") {
        goal.strength = TeamStrength::POWER_PLAY_PENALTY_SHOT;
    } else if(strength == "ShortHandedEmptyNet") {
        goal.strength = TeamStrength::SHORT_HANDED_EMPTY_NET;
    } else if(strength == "ShortHandedPenaltyShot") {
        goal.strength = TeamStrength::SHORT_HANDED_PENALTY_SHOT;
    } else if(strength == "PowerPlayEmptyNet") {
        goal.strength = TeamStrength::POWER_PLAY_EMPTY_NET;
    } else if(strength == "EvenPenaltyShot") {
        goal.strength = TeamStrength::EVEN_PENALTY_SHOT;
    } else if(strength == "PenaltyShot") {
        goal.strength = TeamStrength::PENALTY_SHOT;
    } else {
        std::abort();
    }
}

void from_json(const json& j, IntResult& value_pair) {
    j.at("home").get_to(value_pair.home);
    j.at("away").get_to(value_pair.away);
}

void from_json(const json& j, Game& game) {
    j.at("game_info").get_to(game.game_info);
    j.at("goals").get_to(game.goals);
    j.at("winning_team").get_to(game.winning_team);
    j.at("final_score").at("away").get_to(game.final_result.away);
    j.at("final_score").at("home").get_to(game.final_result.home);
    j.at("shots").get_to(game.shots);
    j.at("power_plays").at("away").at("goals").get_to(game.power_play.away.goals);
    j.at("power_plays").at("away").at("total").get_to(game.power_play.away.attempts);
    j.at("power_plays").at("home").at("goals").get_to(game.power_play.home.goals);
    j.at("power_plays").at("home").at("total").get_to(game.power_play.home.attempts);
    j.at("take_aways").get_to(game.take_aways);
    j.at("give_aways").get_to(game.give_aways);
    j.at("face_offs").at("away").get_to(game.face_off.away);
    j.at("face_offs").at("home").get_to(game.face_off.home);
    j.at("goalie_probably_pulled").get_to(game.goalie_probably_pulled);
}
