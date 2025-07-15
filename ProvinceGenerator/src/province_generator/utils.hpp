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

inline std::pair<int, int> search_spiral(const int x, const int y,
                                         const std::function<bool(int, int)> &valid_pixel, const int max_radius = 10) {
    int current_x = x, current_y = y - 1;
    int current_radius = 1;
    std::pair current_direction = {1, 1};
    while (current_radius <= max_radius) {
        if (valid_pixel(current_x, current_y)) {
            return {current_x, current_y};
        }
        current_x += current_direction.first;
        current_y += current_direction.second;
        if (current_x - x > current_radius || current_y - y > current_radius || x - current_x > current_radius) {
            current_x -= current_direction.first;
            current_y -= current_direction.second;
            current_direction = {-current_direction.second, current_direction.first};
            current_x += current_direction.first;
            current_y += current_direction.second;
        } else if (y - current_y > current_radius) {
            current_x -= 1;
            current_direction = {-current_direction.second, current_direction.first};
            current_radius++;
        }
    }
    return {-1, -1};
}
