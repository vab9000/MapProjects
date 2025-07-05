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
#include "../features/province.hpp"
#include "../features/tag.hpp"
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
        uint_fast32_t color;
        province_file >> color;
        std::getline(province_file, ignore, ':');
        uint_fast32_t koppen_value;
        province_file >> koppen_value;
        std::getline(province_file, ignore, ':');
        uint_fast32_t elevation_value;
        province_file >> elevation_value;
        std::getline(province_file, ignore, ':');
        uint_fast32_t vegetation_value;
        province_file >> vegetation_value;
        std::getline(province_file, ignore, ':');
        uint_fast32_t soil_value;
        province_file >> soil_value;
        std::getline(province_file, ignore);
        color = flip_rb(color);
        auto new_tag = std::make_unique<tag>(color);
        auto id = new_tag->id;
        data.tags.emplace(id, std::move(new_tag));
        data.provinces.emplace(std::piecewise_construct, std::forward_as_tuple(color), std::forward_as_tuple(
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
        uint_fast32_t color;
        sea_file >> color;
        std::getline(sea_file, ignore, ':');
        uint_fast32_t sea_value;
        sea_file >> sea_value;
        color = flip_rb(color);
        data.provinces.emplace(std::piecewise_construct, std::forward_as_tuple(color), std::forward_as_tuple(
                                   color, koppen_t::none, elevation_t::none, vegetation_t::none,
                                   soil_t::none, static_cast<sea_t>(flip_rb(sea_value))));
    }

    auto process_pixel = [&](const uint_fast32_t color, const std::array<uint_fast32_t, 2> position) {
        auto &province = data.provinces.at(color);
        province.expand_bounds(position[0], position[1]);
    };

    std::unordered_map<province *, std::vector<std::array<uint_fast32_t, 2> > > pixels_by_province;

    auto process_pixel_borders = [&](const uint_fast32_t color, const std::array<uint_fast32_t, 2> position) {
        const auto i = static_cast<int_fast32_t>(position[0]);
        const auto j = static_cast<int_fast32_t>(position[1]);

        constexpr std::array<std::array<int_fast8_t, 2>, 8> directions = {
            {
                {-1, -1}, {-1, 0}, {-1, 1},
                {0, -1}, {0, 1},
                {1, -1}, {1, 0}, {1, 1}
            }
        };
        auto &this_province = data.provinces.at(color);
        pixels_by_province[&this_province].push_back(position);

        for (int_fast8_t index = 0; index < 4; index++) {
            const auto &direction = directions[index];
            const auto ni = i + direction[0];
            const auto nj = j + direction[1];
            if (ni < 0 || ni >= map_image.width() ||
                nj < 0 || nj >= map_image.height()) {
                continue;
            }
            const auto neighbor_color = map_image.color(ni, nj);
            if (neighbor_color == color) {
                continue;
            }
            auto &neighbor = data.provinces.at(neighbor_color);
            this_province.add_neighbor(&neighbor);
        }
        if (this_province.sea() == sea_t::none) return;
        for (int_fast8_t index = 4; index < 8; index++) {
            const auto &direction = directions[index];
            const auto ni = i + direction[0];
            const auto nj = j + direction[1];
            if (ni < 0 || ni >= map_image.width() ||
                nj < 0 || nj >= map_image.height()) {
                continue;
            }
            const auto neighbor_color = map_image.color(ni, nj);
            if (neighbor_color == color) {
                continue;
            }
            auto &neighbor = data.provinces.at(neighbor_color);
            if (neighbor.sea() == sea_t::none) continue;
            this_province.add_neighbor(&neighbor);
        }
    };

    map_image = image{"assets/provinces_generated.png"};

    for (uint_fast32_t i = 0; i < map_image.width(); ++i) {
        for (uint_fast32_t j = 0; j < map_image.height(); ++j) {
            const std::array coords = {i, j};
            process_pixel(map_image.color(i, j), coords);
        }

        progress = static_cast<double>(i) / map_image.width() / 2;
    }

    for (uint_fast32_t i = 0; i < map_image.width(); ++i) {
        for (uint_fast32_t j = 0; j < map_image.height(); ++j) {
            const std::array coords = {i, j};
            process_pixel_borders(map_image.color(i, j), coords);
        }

        progress = static_cast<double_t>(i) / map_image.width() / 2 + 0.5;
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
