//
// Created by Simon Farre on 2021-02-07.
//

#pragma once
#include <cstdint>
#include <fmt/core.h>

using u8    = std::uint8_t;
using u16   = std::uint16_t;
using u32   = std::uint32_t;
using u64   = std::uint64_t;

using i8    = std::int_fast8_t;
using i16   = std::int16_t;
using i32   = std::int32_t;
using i64   = std::int64_t;

using usize = std::uintmax_t;

template<typename... Args>
void println(const char *fmt_string, Args... args) {
    fmt::print(fmt_string, args...);
    fmt::print("\n");
    fflush(stdout);
}