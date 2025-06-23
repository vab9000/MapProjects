#pragma once

#include "../common/image.hpp"

inline bool is_water(const image::image_color &color) {
    constexpr unsigned int ocean_color = 0x00FFFF;
    constexpr unsigned int lake_color = 0xFF00FF;

    const auto color_value = static_cast<unsigned int>(color);

    return color_value == ocean_color || color_value == lake_color || (
               color.b() != 0 && color.r() == 0 && color.g() == 0);
}

inline bool is_river(const image::image_color &color) {
    return color.r() == 0 && color.g() == 0 && color.b() != 0;
}

inline int elevation(const image::image_color &color) {
    if (is_water(color)) {
        return -1;
    }
    return color.r() + color.g() + color.b();
}