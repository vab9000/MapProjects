#pragma once

#include "../common/image.hpp"
#include <vector>
#include <random>
#include <array>
#include <iostream>
#include <chrono>
#include <map>
#include <set>
#include "utils.hpp"

inline void generate_provinces(const image &w_img, const image &base_map) {
    constexpr int province_density = 10;
    constexpr int null_province = 0;
    int next_id = 1;
    int filled_pixels = 0;

    std::random_device rd;
    std::default_random_engine rng(rd());
    std::uniform_real_distribution dis(0.0, 1.0);

    std::vector provinces(base_map.width(),
                          std::vector<std::pair<int, std::pair<bool, bool> > >(
                              base_map.height(), {null_province, {false, false}}));

    for (int i = province_density / 2; i < base_map.width(); i += province_density) {
        for (int j = province_density / 2; j < base_map.height(); j += province_density) {
            if (is_water(base_map(i, j))) {
                std::pair position = {i, j + 1};
                std::pair direction = {-1, 0};
                constexpr int max_radius = province_density / 4;
                int current_radius = 1;
                while (true) {
                    if (!(position.first < 0 || position.first >= base_map.width() ||
                          position.second < 0 || position.second >= base_map.height())) {
                        if (provinces[position.first][position.second].first == null_province &&
                            !is_water(base_map(position.first, position.second))) {
                            provinces[position.first][position.second].first = next_id;
                            filled_pixels++;
                            goto increment;
                        }
                    }
                    if (std::abs(position.first - i + direction.first) > current_radius
                        || std::abs(position.second - j + direction.second) > current_radius) {
                        if (direction == std::make_pair(0, 1)) {
                            current_radius++;
                            position.first++;
                            position.second++;
                            direction = std::make_pair(-1, 0);
                        } else {
                            auto [x, y] = direction;
                            direction.first = -y;
                            direction.second = x;
                        }
                    } else {
                        position.first += direction.first;
                        position.second += direction.second;
                    }

                    if (current_radius > max_radius) {
                        break;
                    }
                }
            } {
                std::pair new_position = {
                    i + static_cast<int>(dis(rng) * province_density / 2 - province_density / 4.0),
                    j + static_cast<int>(dis(rng) * province_density / 2 - province_density / 4.0)
                };
                if (new_position.first < 0 || new_position.first >= base_map.width() ||
                    new_position.second < 0 || new_position.second >= base_map.height()) {
                    new_position = {i, j};
                }
                if (is_water(base_map(new_position.first, new_position.second))) {
                    new_position = {i, j};
                }
                provinces[new_position.first][new_position.second].first = next_id;
            }
        increment:
            next_id++;
            filled_pixels++;
        }
    }

    enum class stage {
        land,
        rivers,
        ocean,
    };

    auto current_stage = stage::land;
    bool do_corners = false;
loop:
    // ReSharper disable once CppDFAEndlessLoop
    std::pair recent_changed_pixel = {0, 0};
    auto start = std::chrono::high_resolution_clock::now();
    double nudge = 0.8;

    bool changed = true;

    while (changed) {
        nudge += 0.005;

        changed = false;

        std::cout << "time taken: " << std::chrono::high_resolution_clock::now() - start << " Filled pixels: " <<
                filled_pixels << " Recent Changed Pixel: " << recent_changed_pixel.first << ", " << recent_changed_pixel
                .second << std::endl;
        start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < base_map.width(); ++i) {
            for (int j = 0; j < base_map.height(); ++j) {
                if (provinces[i][j].second.second) {
                    continue;
                }
                if (provinces[i][j].first == null_province) {
                    continue;
                }
                if (current_stage == stage::land && is_water(base_map(i, j))) {
                    continue;
                }
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

                bool empty_neighbor = false;

                for (int index = 0; index < (do_corners ? 8 : 4); ++index) {
                    const auto &[x, y] = directions[index];
                    const std::pair neighbor = {i + x, j + y};
                    if (neighbor.first < 0 || neighbor.first >= base_map.width() ||
                        neighbor.second < 0 || neighbor.second >= base_map.height()) {
                        continue;
                    }

                    if (provinces[neighbor.first][neighbor.second].first != null_province) {
                        continue;
                    }

                    if (current_stage == stage::land && is_water(base_map(neighbor.first, neighbor.second))) {
                        continue;
                    }

                    if (current_stage == stage::rivers && is_water(base_map(neighbor.first, neighbor.second)) &&
                        !is_river(base_map(neighbor.first, neighbor.second))) {
                        continue;
                    }

                    const int this_elevation = elevation(base_map(i, j));
                    const int neighbor_elevation = elevation(base_map(neighbor.first, neighbor.second));

                    const int elevation_difference = this_elevation < 0 || neighbor_elevation < 0
                                                         ? 20
                                                         : std::abs(this_elevation - neighbor_elevation);

                    if (constexpr int randomness = 2;
                        std::pow(dis(rng), nudge) < 1.0 / (randomness + elevation_difference * elevation_difference)) {
                        provinces[neighbor.first][neighbor.second].first = provinces[i][j].first;
                        filled_pixels++;
                        recent_changed_pixel = neighbor;
                        if (x > 0 || y > 0) {
                            provinces[neighbor.first][neighbor.second].second.first = true;
                        }
                    } else {
                        empty_neighbor = true;
                    }

                    changed = true;
                }

                if (!empty_neighbor) {
                    provinces[i][j].second.second = true;
                }
            }
        }
    }

    // ReSharper disable once CppDFAConstantConditions
    if (current_stage != stage::ocean) {
        if (!do_corners && current_stage == stage::land) {
            for (int i = 0; i < base_map.width(); ++i) {
                for (int j = 0; j < base_map.height(); ++j) {
                    provinces[i][j].second = {false, false};
                }
            }

            do_corners = true;
            goto loop;
        }
        do_corners = false;
        current_stage = current_stage == stage::land ? stage::rivers : stage::ocean;
        std::cout << "Water provinces generation started." << std::endl;

        for (int i = 0; i < base_map.width(); ++i) {
            for (int j = 0; j < base_map.height(); ++j) {
                provinces[i][j].second = {false, false};
            }
        }

        goto loop;
    }

    // ReSharper disable once CppDFAUnreachableCode
    std::map<int, int> province_sizes;
    for (int i = 0; i < base_map.width(); ++i) {
        for (int j = 0; j < base_map.height(); ++j) {
            if (!province_sizes.contains(provinces[i][j].first)) {
                province_sizes[provinces[i][j].first] = 0;
            }
            if (is_water(base_map(i, j))) {
                continue;
            }
            province_sizes[provinces[i][j].first]++;
        }
    }

    std::map<int, int> province_merges;
    for (int i = 0; i < base_map.width(); ++i) {
        for (int j = 0; j < base_map.height(); ++j) {
            if (constexpr int min_province_size = 25; !province_merges.contains(provinces[i][j].first) &&
                                                      province_sizes[provinces[i][j].first] < min_province_size) {
                if (is_water(base_map(i, j))) {
                    continue;
                }
                constexpr std::array directions = {
                    std::make_pair(0, -1), std::make_pair(-1, 0), std::make_pair(1, 0), std::make_pair(0, 1)
                };
                for (const auto &[x, y]: directions) {
                    const std::pair neighbor = {i + x, j + y};
                    if (neighbor.first < 0 || neighbor.first >= base_map.width() ||
                        neighbor.second < 0 || neighbor.second >= base_map.height()) {
                        continue;
                    }
                    if (is_water(base_map(neighbor.first, neighbor.second))) {
                        continue;
                    }
                    if (provinces[i][j].first == provinces[neighbor.first][neighbor.second].first) {
                        continue;
                    }
                    if (province_merges.contains(provinces[neighbor.first][neighbor.second].first)) {
                        province_merges[provinces[i][j].first] = province_merges[provinces[neighbor.first][neighbor.
                            second].first];
                        provinces[i][j].first = province_merges[provinces[neighbor.first][neighbor.second].first];
                        break;
                    }
                    province_merges[provinces[i][j].first] = provinces[neighbor.first][neighbor.second].first;
                    break;
                }
            }
        }
    }

    for (int i = 0; i < base_map.width(); ++i) {
        for (int j = 0; j < base_map.height(); ++j) {
            if (province_merges.contains(provinces[i][j].first)) {
                provinces[i][j].first = province_merges[provinces[i][j].first];
            }
        }
    }

    std::map<int, int> province_colors;
    std::set<int> used_colors;
    int id = 1;
    while (province_colors.size() < next_id) {
        while (true) {
            const int random_value = static_cast<int>(dis(rng) * (256 * 256 * 256 - 1));
            if (used_colors.contains(random_value)) {
                continue;
            }
            used_colors.insert(random_value);
            province_colors[id] = random_value;
            break;
        }
        id++;
    }

    for (int i = 0; i < base_map.width(); ++i) {
        for (int j = 0; j < base_map.height(); ++j) {
            if (is_water(base_map(i, j)) && !is_river(base_map(i, j))) {
                w_img(i, j).r() = 255;
                w_img(i, j).g() = 255;
                w_img(i, j).b() = 255;
            } else {
                w_img(i, j).r() = province_colors[provinces[i][j].first] % 256;
                w_img(i, j).g() = (province_colors[provinces[i][j].first] / 256) % 256;
                w_img(i, j).b() = (province_colors[provinces[i][j].first] / (256 * 256)) % 256;
            }
        }
    }
}
