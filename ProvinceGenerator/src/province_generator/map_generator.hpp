#pragma once

#include <array>
#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <unordered_set>
#include <vector>
#include "../common/image.hpp"

#include "utils.hpp"

static std::random_device rd;
static std::default_random_engine rng(rd());
static std::uniform_real_distribution dis(0.0, 1.0);

constexpr auto province_density = 13;
constexpr auto null_province = 0;

using provinces_t = std::vector<std::vector<std::pair<int, std::pair<bool,
    bool>>>>;

inline auto seed_provinces(int &filled_pixels, const image &base_map, provinces_t &provinces) -> int {
    auto next_id = 1;

    for (int i = province_density / 2; i < base_map.width(); i += province_density) {
        for (int j = province_density / 2; j < base_map.height(); j += province_density) {
            if (is_water(base_map(i, j))) {
                std::pair position = {i, j + 1};
                std::pair direction = {-1, 0};
                constexpr int max_radius = province_density / 4;
                auto current_radius = 1;
                while (true) {
                    if (!(position.first < 0 || position.first >= base_map.width() ||
                          position.second < 0 || position.second >= base_map.height())) {
                        if (provinces[position.first][position.second].first == null_province &&
                            !is_water(base_map(position.first, position.second))) {
                            provinces[position.first][position.second].first = next_id;
                            filled_pixels++;
                            break;
                        }
                    }
                    if (std::abs(position.first - i + direction.first) > current_radius
                        || std::abs(position.second - j + direction.second) > current_radius) {
                        if (direction == std::make_pair(0, 1)) {
                            current_radius++;
                            position.first++;
                            position.second++;
                            direction = std::make_pair(-1, 0);
                        }
                        else {
                            auto [x, y] = direction;
                            direction.first = -y;
                            direction.second = x;
                        }
                    }
                    else {
                        position.first += direction.first;
                        position.second += direction.second;
                    }

                    if (current_radius > max_radius) {
                        std::pair new_position = {
                            i + static_cast<int>(dis(rng) * province_density / 2 - province_density / 4.0),
                            j + static_cast<int>(dis(rng) * province_density / 2 - province_density / 4.0)
                        };
                        if (new_position.first < 0 || new_position.first >= base_map.width() ||
                            new_position.second < 0 || new_position.second >= base_map.height()) {
                            new_position = {i, j};
                        }
                        if (is_water(base_map(new_position.first, new_position.second))) { new_position = {i, j}; }
                        provinces[new_position.first][new_position.second].first = next_id;
                        break;
                    }
                }
            }
            else {
                std::pair new_position = {
                    i + static_cast<int>(dis(rng) * province_density / 2 - province_density / 4.0),
                    j + static_cast<int>(dis(rng) * province_density / 2 - province_density / 4.0)
                };
                if (new_position.first < 0 || new_position.first >= base_map.width() ||
                    new_position.second < 0 || new_position.second >= base_map.height()) { new_position = {i, j}; }
                if (is_water(base_map(new_position.first, new_position.second))) { new_position = {i, j}; }
                provinces[new_position.first][new_position.second].first = next_id;
            }
            next_id++;
            filled_pixels++;
        }
    }
    return next_id;
}

enum class stage {
    land, rivers, ocean,
};

inline auto fill_provinces(int &filled_pixels, const image &base_map, provinces_t &provinces, const stage current_stage,
    const bool do_corners) -> void {
    auto changed = true;
    auto nudge = 0.8;

    std::pair recent_changed_pixel = {0, 0};
    auto start = std::chrono::high_resolution_clock::now();

    while (changed) {
        nudge += 0.01;

        changed = false;

        std::cout << "time taken: " << std::chrono::high_resolution_clock::now() - start << " Filled pixels: " <<
            filled_pixels << " Recent Changed Pixel: " << recent_changed_pixel.first << ", " << recent_changed_pixel
            .second << std::endl;
        start = std::chrono::high_resolution_clock::now();

        for (auto i = 0; i < base_map.width(); ++i) {
            for (auto j = 0; j < base_map.height(); ++j) {
                if (provinces[i][j].second.second) { continue; }
                if (provinces[i][j].first == null_province) { continue; }
                if (current_stage == stage::land && is_water(base_map(i, j))) { continue; }
                if (current_stage == stage::rivers && is_water(base_map(i, j)) && !is_river(base_map(i, j))) {
                    continue;
                }
                if (provinces[i][j].second.first) {
                    provinces[i][j].second.first = false;
                    changed = true;
                    continue;
                }

                constexpr std::array directions = {
                    std::make_pair(0, -1), std::make_pair(-1, 0), std::make_pair(1, 0), std::make_pair(0, 1),
                    std::make_pair(-1, -1), std::make_pair(-1, 1), std::make_pair(1, -1), std::make_pair(1, 1)
                };

                auto empty_neighbor = false;

                for (auto index = 0; index < (do_corners ? 8 : 4); ++index) {
                    const auto &[x, y] = directions[index];
                    const std::pair neighbor = {i + x, j + y};
                    if (neighbor.first < 0 || neighbor.first >= base_map.width() ||
                        neighbor.second < 0 || neighbor.second >= base_map.height()) { continue; }

                    if (provinces[neighbor.first][neighbor.second].first != null_province) { continue; }

                    if (current_stage == stage::land && is_water(base_map(neighbor.first, neighbor.second))) {
                        continue;
                    }

                    if (current_stage == stage::rivers && is_water(base_map(neighbor.first, neighbor.second)) &&
                        !is_river(base_map(neighbor.first, neighbor.second))) { continue; }

                    const int this_elevation = elevation(base_map(i, j));
                    const int neighbor_elevation = elevation(base_map(neighbor.first, neighbor.second));

                    const int elevation_difference = this_elevation < 0 || neighbor_elevation < 0
                        ? 20
                        : std::abs(this_elevation - neighbor_elevation);

                    if (constexpr auto randomness = 2;
                        std::pow(dis(rng), nudge) < 1.0 / (randomness + elevation_difference * elevation_difference)) {
                        provinces[neighbor.first][neighbor.second].first = provinces[i][j].first;
                        filled_pixels++;
                        recent_changed_pixel = neighbor;
                        if (x > 0 || y > 0) { provinces[neighbor.first][neighbor.second].second.first = true; }
                    }
                    else { empty_neighbor = true; }

                    changed = true;
                }

                if (!empty_neighbor) { provinces[i][j].second.second = true; }
            }
        }
    }
}

inline auto generate_province_sizes(const image &base_map,
    const provinces_t &provinces) -> std::unordered_map<int, int> {
    std::unordered_map<int, int> province_sizes;
    for (auto i = 0; i < base_map.width(); ++i) {
        for (auto j = 0; j < base_map.height(); ++j) {
            if (!province_sizes.contains(provinces[i][j].first)) { province_sizes[provinces[i][j].first] = 0; }
            if (is_water(base_map(i, j))) { continue; }
            province_sizes[provinces[i][j].first]++;
        }
    }
    return province_sizes;
}

inline auto merge_provinces(const image &base_map, std::unordered_map<int, int> &province_merges,
    const std::unordered_map<int, int> &province_sizes, const provinces_t &provinces,
    const int radius, const int min_province_size) -> void {
    for (auto i = 0; i < base_map.width(); ++i) {
        for (auto j = 0; j < base_map.height(); ++j) {
            if (!province_merges.contains(provinces[i][j].first) &&
                province_sizes.at(provinces[i][j].first) < min_province_size) {
                if (is_water(base_map(i, j))) { continue; }

                const std::function is_valid_pixel = [&](const int x, const int y) {
                    if (x < 0 || x >= base_map.width() ||
                        y < 0 || y >= base_map.height()) { return false; }
                    if (is_water(base_map(x, y))) { return false; }
                    if (provinces[i][j].first == provinces[x][y].first) { return false; }
                    if (province_merges.contains(provinces[x][y].first)) {
                        auto nested_merge = province_merges.at(provinces[x][y].first);
                        while (province_merges.contains(nested_merge)) {
                            nested_merge = province_merges.at(nested_merge);
                        }
                        if (nested_merge == provinces[i][j].first) { return false; }
                    }
                    return true;
                };

                const auto [x, y] = search_spiral(i, j, is_valid_pixel, radius);

                if (x == -1) { continue; }

                if (province_merges.contains(provinces[x][y].first)) {
                    auto nested_merge = province_merges.at(provinces[x][y].first);
                    while (province_merges.contains(nested_merge)) { nested_merge = province_merges.at(nested_merge); }
                    province_merges[provinces[i][j].first] = nested_merge;
                }
                else { province_merges[provinces[i][j].first] = provinces[x][y].first; }
            }
        }
    }
}

inline auto generate_provinces(const image &w_img, const image &base_map) -> void {
    auto filled_pixels = 0;

    provinces_t provinces(base_map.width(),
        std::vector<std::pair<int, std::pair<bool, bool>>>(
            base_map.height(), {null_province, {false, false}}));

    const int num_provinces = seed_provinces(filled_pixels, base_map, provinces);

    auto current_stage = stage::land;
    auto do_corners = false;

    fill_provinces(filled_pixels, base_map, provinces, current_stage, do_corners);

    while (current_stage != stage::ocean) {
        if (!do_corners && current_stage == stage::land) {
            for (auto i = 0; i < base_map.width(); ++i) {
                for (auto j = 0; j < base_map.height(); ++j) { provinces[i][j].second = {false, false}; }
            }

            do_corners = true;
        }
        else {
            do_corners = false;
            current_stage = current_stage == stage::land ? stage::rivers : stage::ocean;
            std::cout << "Water provinces generation started." << std::endl;

            for (auto i = 0; i < base_map.width(); ++i) {
                for (auto j = 0; j < base_map.height(); ++j) { provinces[i][j].second = {false, false}; }
            }
        }

        fill_provinces(filled_pixels, base_map, provinces, current_stage, do_corners);
    }

    const std::unordered_map<int, int> province_sizes = generate_province_sizes(base_map, provinces);

    std::unordered_map<int, int> province_merges;
    merge_provinces(base_map, province_merges, province_sizes, provinces, 1, 50);
    merge_provinces(base_map, province_merges, province_sizes, provinces, 15, 15);

    for (auto i = 0; i < base_map.width(); ++i) {
        for (auto j = 0; j < base_map.height(); ++j) {
            if (province_merges.contains(provinces[i][j].first)) {
                auto merge_province = province_merges.at(provinces[i][j].first);
                while (province_merges.contains(merge_province)) {
                    if (merge_province == provinces[i][j].first) { break; }
                    if (merge_province == province_merges.at(merge_province)) { break; }
                    merge_province = province_merges.at(merge_province);
                }
                provinces[i][j].first = merge_province;
            }
        }
    }

    std::unordered_map<int, int> province_colors;
    std::unordered_set<int> used_colors;
    auto id = 1;
    while (province_colors.size() < num_provinces) {
        while (true) {
            const auto random_value = static_cast<int>(dis(rng) * (256 * 256 * 256 - 1));
            if (used_colors.contains(random_value)) { continue; }
            used_colors.insert(random_value);
            province_colors[id] = random_value;
            break;
        }
        id++;
    }

    for (auto i = 0; i < base_map.width(); ++i) {
        for (auto j = 0; j < base_map.height(); ++j) {
            if (is_water(base_map(i, j)) && !is_river(base_map(i, j))) {
                w_img(i, j).r() = 255;
                w_img(i, j).g() = 255;
                w_img(i, j).b() = 255;
            }
            else {
                w_img(i, j).r() = province_colors[provinces[i][j].first] % 256;
                w_img(i, j).g() = (province_colors[provinces[i][j].first] / 256) % 256;
                w_img(i, j).b() = (province_colors[provinces[i][j].first] / (256 * 256)) % 256;
            }
        }
    }
}
