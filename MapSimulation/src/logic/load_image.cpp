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

void load_image(data &data, image &map_image, double &progress) {
    std::ifstream province_file("assets/provinces.txt");
    if (!province_file.is_open()) {
        throw std::runtime_error("Failed to open province file: " "provinces.txt");
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
        std::getline(province_file, ignore, ':');
        unsigned int soil_value;
        province_file >> soil_value;
        std::getline(province_file, ignore);
        color = flip_rb(color);
        auto new_tag = std::make_unique<tag>(std::string("Tag ") + std::to_string(data.tags.size()), color);
        auto id = new_tag->id;
        data.tags.emplace(id, std::move(new_tag));
        data.provinces.emplace(std::piecewise_construct, std::forward_as_tuple(color), std::forward_as_tuple(std::string("Province ") + std::to_string(data.provinces.size()),
                                               color, static_cast<koppen_t>(flip_rb(koppen_value)),
                                               static_cast<elevation_t>(flip_rb(elevation_value)),
                                               static_cast<vegetation_t>(flip_rb(vegetation_value)),
                                               static_cast<soil_t>(flip_rb(soil_value)), sea_t::none));
        data.provinces[color].set_owner(data.tags[id].get());
    }
    province_file.close();

    std::ifstream sea_file("assets/sea_tiles.txt");
    if (!sea_file.is_open()) {
        throw std::runtime_error("Failed to open sea tiles file: " "sea_tiles.txt");
    }
    while (!sea_file.eof()) {
        std::string ignore;
        std::getline(sea_file, ignore, ':');
        if (ignore == "\n") break;
        unsigned int color;
        sea_file >> color;
        std::getline(sea_file, ignore, ':');
        unsigned int sea_value;
        sea_file >> sea_value;
        color = flip_rb(color);
        data.provinces.emplace(std::piecewise_construct, std::forward_as_tuple(color), std::forward_as_tuple(std::string("Sea Province") + std::to_string(data.provinces.size()),
                                               color, koppen_t::none, elevation_t::none, vegetation_t::none,
                                               soil_t::none, static_cast<sea_t>(flip_rb(sea_value))));
    }

    auto process_pixel = [&](const unsigned int color, const std::array<int, 2> position) {
        const auto i = position[0];
        const auto j = position[1];

        auto &province = data.provinces.at(color);
        province.expand_bounds(i, j);
    };

    auto is_border = [&](const int x, const int y, const unsigned int color, province *&other_province) {
        if (x >= 0 && x < map_image.width() && y >= 0 && y < map_image.height() && map_image.color(x, y) !=
            color) {
            other_province = &data.provinces.at(map_image.color(x, y));
            return true;
        }
        if (x == -1 && y >= 0 && y < map_image.height() && map_image.color(map_image.width() - 1, y) !=
            color) {
            other_province = &data.provinces.at(map_image.color(map_image.width() - 1, y));
            return true;
        }
        if (x == map_image.width() && y >= 0 && y < map_image.height() && map_image.color(0, y) != color) {
            other_province = &data.provinces.at(map_image.color(0, y));
            return true;
        }
        return false;
    };

    std::unordered_map<province *, std::vector<std::array<int, 2> > > pixels_by_province;

    auto process_pixel_borders = [&](const unsigned int color, const std::array<int, 2> position) {
        const auto i = position[0];
        const auto j = position[1];
        province *province_at_pos = &data.provinces.at(color);

        if (province *other_province = nullptr;
            is_border(i - 1, j, color, other_province) || is_border(i + 1, j, color, other_province) ||
            is_border(i, j - 1, color, other_province) || is_border(i, j + 1, color, other_province)) {
            province_at_pos->add_neighbor(other_province);
        }
        pixels_by_province[province_at_pos].push_back(std::array{i, j});
    };

    map_image = image{"assets/provinces_generated.png"};

    for (int i = 0; i < map_image.width(); ++i) {
        for (int j = 0; j < map_image.height(); ++j) {
            const std::array coords = {i, j};
            process_pixel(map_image.color(i, j), coords);
        }

        progress = static_cast<double>(i) / map_image.width() / 2;
    }

    for (int i = 0; i < map_image.width(); ++i) {
        for (int j = 0; j < map_image.height(); ++j) {
            const std::array coords = {i, j};
            process_pixel_borders(map_image.color(i, j), coords);
        }

        progress = static_cast<double>(i) / map_image.width() / 2 + 0.5;
    }

    auto province_values = data.provinces | std::views::values;

#ifdef __cpp_lib_execution
    std::for_each(std::execution::par_unseq, province_values.begin(), province_values.end(),
                  [&pixels_by_province](province &province) { province.finalize(pixels_by_province.at(&province)); });
    std::for_each(std::execution::par_unseq, province_values.begin(), province_values.end(),
                  [](province &province) { province.process_distances(); });
#else
    std::ranges::for_each(province_values,
                          [&pixels_by_province](province &province) { province.finalize(pixels_by_province.at(&province)); });
    std::ranges::for_each(province_values,
                          [](province &province) { province.process_distances(); });
#endif
}
