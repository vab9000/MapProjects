#include "load_image.hpp"

#include <algorithm>
#include <memory>
#include <fstream>
#include <ranges>
#include <iostream>
#ifdef __cpp_lib_execution
#include <execution>
#endif
#include "image.hpp"
#include "../features/base/province.hpp"
#include "../features/tags/tag.hpp"
#include "data.hpp"

constexpr unsigned int flip_rb(const unsigned int color) {
    return (color & 0x00FF00) | ((color & 0xFF0000) >> 16) | ((color & 0x0000FF) << 16);
}

void load_image(data &data, image &map_image, double &progress, const std::string &province_filepath) {
    std::ifstream province_file(province_filepath);
    if (!province_file.is_open()) {
        throw std::runtime_error("Failed to open province file: " + province_filepath);
    }
    while (!province_file.eof()) {
        std::string ignore;
        std::getline(province_file, ignore, ':');
        if (ignore == "\n") break;
        unsigned int color;
        province_file >> color;
        std::getline(province_file, ignore, ':');
        unsigned int koppen_value;
        province_file >> koppen_value;
        std::getline(province_file, ignore, ':');
        unsigned int elevation_value;
        province_file >> elevation_value;
        std::getline(province_file, ignore, ':');
        unsigned int vegetation_value;
        province_file >> vegetation_value;
        std::getline(province_file, ignore);
        color = flip_rb(color);
        auto new_tag = std::make_unique<tag>(std::string("Tag ") + std::to_string(data.tags.size()), color);
        auto id = new_tag->id;
        data.tags.emplace(id, std::move(new_tag));
        data.provinces.emplace(color, province(std::string("Province ") + std::to_string(data.provinces.size()),
                                               color, static_cast<koppen>(flip_rb(koppen_value)),
                                               static_cast<elevation>(flip_rb(elevation_value)),
                                               static_cast<vegetation>(flip_rb(vegetation_value))));
        data.provinces[color].set_owner(*data.tags[id]);
    }
    province_file.close();

    auto process_pixel = [&](const unsigned int color, const std::array<int, 2> position) {
        const auto i = position[1];
        const auto j = position[0];

        const auto province = &data.provinces.at(color);
        province->expand_bounds(i, j);
    };

    auto is_border = [&](const int x, const int y, const unsigned int color, province *&other_province) {
        if (x >= 0 && x < map_image.get_width() && y >= 0 && y < map_image.get_height() && map_image.get_color(x, y) !=
            color) {
            other_province = &data.provinces.at(map_image.get_color(x, y));
            return true;
        }
        if (x == -1 && y >= 0 && y < map_image.get_height() && map_image.get_color(map_image.get_width() - 1, y) !=
            color) {
            other_province = &data.provinces.at(map_image.get_color(map_image.get_width() - 1, y));
            return true;
        }
        if (x == map_image.get_width() && y >= 0 && y < map_image.get_height() && map_image.get_color(0, y) != color) {
            other_province = &data.provinces.at(map_image.get_color(0, y));
            return true;
        }
        return false;
    };

    auto process_pixel_borders = [&](const unsigned int color, const std::array<int, 2> position) {
        const auto i = position[1];
        const auto j = position[0];
        province *province_at_pos = &data.provinces.at(color);

        if (province *other_province = nullptr;
            is_border(i - 1, j, color, other_province) || is_border(i + 1, j, color, other_province) ||
            is_border(i, j - 1, color, other_province) || is_border(i, j + 1, color, other_province)) {
            province_at_pos->add_outline(i, j, *other_province);
        } else {
            province_at_pos->add_pixel(i, j);
        }
    };

    map_image = image{"assets/provinces_generated.png"};

    for (int i = 0; i < map_image.get_width(); ++i) {
        for (int j = 0; j < map_image.get_height(); ++j) {
            const std::array coords = {j, i};
            process_pixel(map_image.get_color(i, j), coords);
        }

        progress = static_cast<double>(i) / map_image.get_width() / 2;
    }

    for (int i = 0; i < map_image.get_width(); ++i) {
        for (int j = 0; j < map_image.get_height(); ++j) {
            const std::array coords = {j, i};
            process_pixel_borders(map_image.get_color(i, j), coords);
        }

        progress = static_cast<double>(i) / map_image.get_width() / 2 + 0.5;
    }

    auto province_values = data.provinces | std::views::values;

#ifdef __cpp_lib_execution
    std::for_each(std::execution::par_unseq, province_values.begin(), province_values.end(),
                  [](province &province) { province.finalize(); });
    std::for_each(std::execution::par_unseq, province_values.begin(), province_values.end(),
                  [](province &province) { province.process_distances(); });
#else
    std::ranges::for_each(province_values,
                          [](province &province) { province.finalize(); });
    std::ranges::for_each(province_values,
                          [](province &province) { province.process_distances(); });
#endif
}
