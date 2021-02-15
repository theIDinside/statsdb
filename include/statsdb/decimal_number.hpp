//
// Created by Simon Farre: 2021-02-15.
// Github: https://github.com/theIDinside
//

#pragma once

/// The wonderful magic of constexpr. pow_10<3> compiles into: "mov eax, 1000" which is exactly what we want
template<int Exponent>
constexpr auto pow_10() {
    auto r = 1;
    for (auto i = 0; i < Exponent; i++) r *= 10;
    return r;
}

/// Utility class. Templated on the amount of decimal points / what accuracy the number is. can implicitly be converted to/from floats
/// So DecimalNumber<2> num = 0.547f -> 0.55 and DecimalNumber<1> num = 0.547 -> 0.5
template<int DecimalPoints>
struct RoundedDecimalNumber {
    constexpr RoundedDecimalNumber() : value_{0.0f} {}
    constexpr RoundedDecimalNumber(float f) {
        float pow = pow_10<DecimalPoints>();
        float tmp = std::round(f * pow);
        value_ = float(tmp) / pow;
    }
    constexpr operator float() { return value_; }
    constexpr float value() { return value_; }
    float value_;
};

using Number = RoundedDecimalNumber<2>;
using RollingStandard = std::vector<Number>;