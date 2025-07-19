#pragma once
#include <cstdint>

namespace utils {
    constexpr auto flip_rb(const uint_fast32_t color) -> uint_fast32_t {
        return (color & 0x00FF00) | ((color & 0xFF0000) >> 16) | ((color & 0x0000FF) << 16);
    }

    constexpr auto to_integer_color(const uint8_t r, const uint8_t g, const uint8_t b) -> uint_fast32_t {
        return flip_rb(
            (static_cast<uint_fast32_t>(r) << 16) | (static_cast<uint_fast32_t>(g) << 8) | static_cast<uint_fast32_t>(
                b));
    }
}
