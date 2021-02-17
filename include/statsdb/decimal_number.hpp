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
/// This class is basically used when we want to display numbers. If you want to do math with them, be-fucking-wary
template<int DecimalPoints>
struct RoundedDecimalNumber {
    static_assert(DecimalPoints > 0, "Template parameter DecimalPoints error: Decimal 'precision' must be > 0");
    constexpr RoundedDecimalNumber() : value_{0.0f} {}
    constexpr RoundedDecimalNumber(float f) : value_(f) {

    }
    constexpr operator float() const {
        float pow = pow_10<DecimalPoints>();
        float tmp = std::round(value_ * pow);
        return float(tmp) / pow;
    }
    constexpr float value() const {
        float pow = pow_10<DecimalPoints>();
        float tmp = std::round(value_ * pow);
        return float(tmp) / pow;
    }
    static constexpr auto precision() { return PRECISION; }
private:
    float value_;
    static constexpr int PRECISION = DecimalPoints;


    template <int DPL, int DPR>
    friend constexpr auto operator+(RoundedDecimalNumber<DPL>, RoundedDecimalNumber<DPR>);
    template<int DPL, int DPR>
    friend constexpr auto operator-(RoundedDecimalNumber<DPL>, RoundedDecimalNumber<DPR>);
    template <int DP>
    friend constexpr auto operator+(RoundedDecimalNumber<DP>, float);
    template<int DP>
    friend constexpr auto operator-(RoundedDecimalNumber<DP>, float);
    template <int DP>
    friend constexpr auto operator+(float, RoundedDecimalNumber<DP>);
    template<int DP>
    friend constexpr auto operator-(float, RoundedDecimalNumber<DP>);
};

template <int DPL, int DPR>
constexpr auto operator+(RoundedDecimalNumber<DPL> lhs, RoundedDecimalNumber<DPR> rhs) {
    if constexpr(RoundedDecimalNumber<DPL>::PRECISION < RoundedDecimalNumber<DPR>::PRECISION) {
        return RoundedDecimalNumber<DPR>{lhs.value_ + rhs.value_};
    } else {
        return RoundedDecimalNumber<DPL>{lhs.value_ + rhs.value_};
    }
}

template <int DPL, int DPR>
constexpr auto operator-(RoundedDecimalNumber<DPL> lhs, RoundedDecimalNumber<DPR> rhs) {
    if constexpr(RoundedDecimalNumber<DPL>::PRECISION < RoundedDecimalNumber<DPR>::PRECISION) {
        return RoundedDecimalNumber<DPR>{lhs.value_ + rhs.value_};
    } else {
        return RoundedDecimalNumber<DPL>{lhs.value_ + rhs.value_};
    }
}

template <int DP>
constexpr auto operator+(RoundedDecimalNumber<DP> lhs, float rhs) {
    return RoundedDecimalNumber<DP>{lhs.value_ + rhs};
}
template<int DP>
constexpr auto operator-(RoundedDecimalNumber<DP> lhs, float rhs) {
    return RoundedDecimalNumber<DP>{lhs.value_ - rhs};
}
template <int DP>
constexpr auto operator+(float lhs, RoundedDecimalNumber<DP> rhs) {
    return RoundedDecimalNumber<DP>{lhs + rhs.value_};
}
template<int DP>
constexpr auto operator-(float lhs, RoundedDecimalNumber<DP> rhs) {
    return RoundedDecimalNumber<DP>{lhs - rhs.value_};
}

using Number = RoundedDecimalNumber<2>;
using RollingStandard = std::vector<Number>;