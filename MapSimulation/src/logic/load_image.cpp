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
#include "../features/data.hpp"

inline void load_provinces(data &d) {
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
        d.provinces().emplace(std::piecewise_construct, std::forward_as_tuple(color), std::forward_as_tuple(
                                  color, static_cast<koppen_t>(flip_rb(koppen_value)),
                                  static_cast<elevation_t>(flip_rb(elevation_value)),
                                  static_cast<vegetation_t>(flip_rb(vegetation_value)),
                                  static_cast<soil_t>(flip_rb(soil_value)), sea_t::none));
    }
    province_file.close();
}

inline void load_seas(data &d) {
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
        d.provinces().emplace(std::piecewise_construct, std::forward_as_tuple(color), std::forward_as_tuple(
                                  color, koppen_t::none, elevation_t::none, vegetation_t::none,
                                  soil_t::none, static_cast<sea_t>(flip_rb(sea_value))));
    }
    sea_file.close();
}

inline void load_river_borders(data &d) {
    std::ifstream rivers_file("assets/rivers.txt");
    if (!rivers_file.is_open()) {
        throw std::runtime_error("Failed to open rivers file: " "rivers.txt");
    }
    while (!rivers_file.eof()) {
        std::string ignore;
        std::getline(rivers_file, ignore, ':');
        if (ignore == "\n") break;
        uint_fast32_t color;
        rivers_file >> color;
        std::getline(rivers_file, ignore, ':');
        uint_fast32_t neighbor_color;
        rivers_file >> neighbor_color;
        std::getline(rivers_file, ignore, ':');
        uint_fast32_t river_value;
        rivers_file >> river_value;
        color = flip_rb(color);
        neighbor_color = flip_rb(neighbor_color);
        d.provinces().at(color).add_river_boundary(&d.provinces().at(neighbor_color),
                                                   static_cast<uint_fast8_t>(river_value));
    }
    rivers_file.close();
}

void load_river_lines(data &d) {
    std::ifstream river_lines_file("assets/river_lines.txt");
    if (!river_lines_file.is_open()) {
        throw std::runtime_error("Failed to open river lines file: " "river_lines.txt");
    }
    auto *current_river = &d.add_river();
    while (!river_lines_file.eof()) {
        std::string command;
        std::getline(river_lines_file, command, ':');
        if (command == "\n") break;
        if (command == "River ID") {
            current_river = &d.add_river();
            uint_fast32_t river_id;
            river_lines_file >> river_id;
        } else if (command == "Province Color") {
            uint_fast32_t province_color;
            river_lines_file >> province_color;
            province_color = flip_rb(province_color);
            current_river->add_province(&d.provinces().at(province_color));
        }
        std::getline(river_lines_file, command, '\n');
    }
    river_lines_file.close();
}

inline void process_pixel_borders(
    std::unordered_map<province *, std::vector<std::array<uint_fast32_t, 2> > > &pixels_by_province,
    const image &map_image, data &d, const uint_fast32_t color, const std::array<uint_fast32_t, 2> &position) {
    const auto i = static_cast<int_fast32_t>(position[0]);
    const auto j = static_cast<int_fast32_t>(position[1]);

    auto &this_province = d.provinces().at(color);
    pixels_by_province[&this_province].push_back(position);

    std::ranges::for_each(std::views::iota(-1, 1 + 1), [&](const int_fast8_t dx) {
        std::ranges::for_each(std::views::iota(-1, 1 + 1), [&](const int_fast8_t dy) {
            if (dx == 0 && dy == 0) return;
            if (dx != 0 && dy != 0 && this_province.sea() == sea_t::none) return;
            const auto ni = i + dx, nj = j + dy;
            if (ni < 0 || ni >= map_image.width() ||
                nj < 0 || nj >= map_image.height()) {
                return;
            }
            const auto neighbor_color = map_image.color(ni, nj);
            if (neighbor_color == color) {
                return;
            }
            auto &neighbor = d.provinces().at(neighbor_color);
            if (dx != 0 && dy != 0 && neighbor.sea() == sea_t::none) return;
            this_province.add_neighbor(&neighbor);
        });
    });
}

void load_image(data &d, image &map_image, std::string &loading_text) {
    loading_text = "Loading provinces file...";
    load_provinces(d);

    loading_text = "Loading sea tiles file...";
    load_seas(d);

    loading_text = "Loading rivers file...";
    load_river_borders(d);

    loading_text = "Loading river lines file...";
    load_river_lines(d);

    std::unordered_map<province *, std::vector<std::array<uint_fast32_t, 2> > > pixels_by_province;

    map_image = image{"assets/provinces_generated.png"};

    loading_text = "Processing pixels...";
    std::ranges::for_each(std::views::iota(0, map_image.width()), [&](const uint_fast32_t i) {
        std::ranges::for_each(std::views::iota(0, map_image.height()), [&](const uint_fast32_t j) {
            d.provinces().at(map_image.color(i, j)).expand_bounds(i, j);
        });
    });

    loading_text = "Processing pixel borders...";
    std::ranges::for_each(std::views::iota(0, map_image.width()), [&](const uint_fast32_t i) {
        std::ranges::for_each(std::views::iota(0, map_image.height()), [&](const uint_fast32_t j) {
            process_pixel_borders(pixels_by_province, map_image, d, map_image.color(i, j), {i, j});
        });
    });

    auto province_values = d.provinces() | std::views::values;

    loading_text = "Finalizing provinces...";
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
    loading_text.clear();
}
