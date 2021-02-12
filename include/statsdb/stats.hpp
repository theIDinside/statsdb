//
// Created by 46769 on 2021-02-11.
//

#include "game_data.hpp"
#include <string_view>
#include <vector>

/// The wonderful magic of constexpr. pow_10<3> compiles into: "mov eax, 1000" which is exactly what we want
template <int Exponent>
constexpr auto pow_10() {
    auto r = 1;
    for(auto i = 0; i < Exponent; i++) r *= 10;
    return r;
}

template <int DecimalPoints>
struct DecimalNumber {
    constexpr DecimalNumber() : value_{0.0f} {}
    constexpr DecimalNumber(float f) {
        float pow = pow_10<DecimalPoints>();
        float tmp = std::round(f * pow);
        value_ = float(tmp) / pow;
    }
    constexpr operator float() { return value_; }
    constexpr float value() { return value_; }
    float value_;
};

/// Pre-condition of all functions: games.size() >= span. Assertion in debug, in release mode, you're screwed if you don't make sure of this.
namespace span_avg {
    std::vector<DecimalNumber<2>> goals_for(std::string_view team, const std::vector<Game>& games, int span);
    std::vector<DecimalNumber<2>> goals_against(std::string_view team, const std::vector<Game>& games, int span);
    std::vector<DecimalNumber<2>> power_play(std::string_view team, const std::vector<Game>& games, int span);
    std::vector<DecimalNumber<2>> penalty_kill(std::string_view team, const std::vector<Game>& games, int span);
    std::vector<DecimalNumber<2>> times_in_pk(std::string_view team, const std::vector<Game>& games, int span);
    std::vector<DecimalNumber<2>> times_in_pp(std::string_view team, const std::vector<Game>& games, int span);
}

