#pragma once
#include <cstdint>

namespace utils {
    constexpr auto flip_rb(const unsigned int color) -> unsigned int {
        return (color & 0x00FF00U) | ((color & 0xFF0000U) >> 16) | ((color & 0x0000FFU) << 16);
    }

    constexpr auto to_integer_color(const uint8_t r, const uint8_t g, const uint8_t b) -> unsigned int {
        return flip_rb(
            (static_cast<unsigned int>(r) << 16) | (static_cast<unsigned int>(g) << 8) | static_cast<unsigned int>(
                b));
    }
}
