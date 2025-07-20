#pragma once

#include "../common/image.hpp"
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <random>
#include <fstream>
#include <functional>
#include <iostream>

inline std::random_device rd;
inline std::default_random_engine generator(rd());


using sea_tile_id_map = std::vector<std::vector<int> >;

inline int next_id() {
    static int id = 1;
    return id++;
}

inline bool is_lake(const image::image_color &color) {
    return color.r() == 255 && color.g() == 0 && color.b() == 255;
}

struct hash_coords {
    std::size_t operator()(const std::pair<int, int> &p) const noexcept {
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};

inline std::unordered_set<std::pair<int, int>, hash_coords> find_entire_lake(
    const image &base_map, sea_tile_id_map &sea_tile_map, const int x, const int y) {
    std::unordered_set<std::pair<int, int>, hash_coords> lake_tiles;

    const auto empty_lake = [&base_map, &sea_tile_map](const int lx, const int ly) {
        return is_lake(base_map(lx, ly)) && sea_tile_map[lx][ly] == 0;
    };

    std::unordered_set<std::pair<int, int>, hash_coords> search_pixels;
    search_pixels.insert({x, y});

    const auto find_lake = [&lake_tiles, &base_map, &search_pixels, &empty_lake](const int lx, const int ly) {
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) continue;
                if (lx + i < 0 || lx + i >= base_map.width() || ly + j < 0 || ly + j >= base_map.height() || !
                    empty_lake(lx + i, ly + j)) {
                    continue;
                }
                if (lake_tiles.contains({lx + i, ly + j})) continue;
                search_pixels.insert({lx + i, ly + j});
            }
        }
    };

    while (!search_pixels.empty()) {
        auto temp_search = std::move(search_pixels);
        search_pixels = {};
        for (const auto &[lx, ly]: temp_search) {
            if (!lake_tiles.insert({lx, ly}).second) continue;
            find_lake(lx, ly);
        }
    }
    return lake_tiles;
}

inline void generate_lakes(const image &base_map, sea_tile_id_map &sea_tile_map) {
    for (int i = 0; i < base_map.width(); i++) {
        for (int j = 0; j < base_map.height(); j++) {
            if (is_lake(base_map(i, j)) && sea_tile_map[i][j] == 0) {
                const auto lake_tiles = find_entire_lake(base_map, sea_tile_map, i, j);
                if (lake_tiles.empty()) continue;
                const int id = next_id();
                for (const auto &[tx, ty]: lake_tiles) {
                    sea_tile_map[tx][ty] = id;
                }
            }
        }
    }
}

inline bool is_ocean(const image::image_color &color) {
    return color.r() == 0 && color.g() == 255 && color.b() == 255;
}

inline bool is_land(const image::image_color &color) {
    if (color.r() != 255 && color.g() == 0 && color.b() == 0) return true;
    if (color.r() == 255 && color.g() != 255 && color.b() == 0) return true;
    if (color.r() == 255 && color.g() == 255) return true;
    return false;
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

inline void spread_ocean_tiles(const image &base_map, sea_tile_id_map &sea_tile_map, const bool spread_from_land, const bool do_corners,
                               int &filled_pixels) {
    std::uniform_real_distribution dis(0.0, 1.0);
    std::vector finished(base_map.width(), std::vector(base_map.height(), std::pair(false, false)));
    bool changed = true;
    while (changed) {
        changed = false;
        std::cout << "Spreading ocean tiles, filled pixels: " << filled_pixels << std::endl;
        for (int i = 0; i < base_map.width(); i++) {
            for (int j = 0; j < base_map.height(); j++) {
                if (finished[i][j].second) {
                    finished[i][j].second = false;
                    changed = true;
                    continue;
                }
                if (finished[i][j].first) continue;
                if (sea_tile_map[i][j] == 0) continue;
                if (is_lake(base_map(i, j))) continue;
                if (!spread_from_land && is_land(base_map(i, j))) continue;
                finished[i][j].first = true;
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        if (dx == 0 && dy == 0) continue;
                        if (do_corners && (dx != 0 && dy != 0)) continue;
                        const int ni = i + dx;
                        const int nj = j + dy;
                        if (ni < 0 || ni >= base_map.width() || nj < 0 || nj >= base_map.height()) continue;
                        if (sea_tile_map[ni][nj] != 0) continue;
                        finished[i][j].first = false;
                        if (dis(generator) < 0.5) {
                            sea_tile_map[ni][nj] = sea_tile_map[i][j];
                            finished[ni][nj].second = true;
                            filled_pixels++;
                        }
                        changed = true;
                    }
                }
            }
        }
    }
}

inline void generate_ocean_tiles(const image &base_map, sea_tile_id_map &sea_tile_map) {
    constexpr int tile_size = 50;
    std::uniform_int_distribution dis(-tile_size / 3, tile_size / 3);

    for (int i = tile_size / 2; i < base_map.width(); i += tile_size) {
        for (int j = tile_size / 2; j < base_map.height(); j += tile_size) {
            const auto ni = i + dis(generator);
            const auto nj = j + dis(generator);
            if (sea_tile_map[ni][nj] != 0) {
                continue;
            }
            sea_tile_map[ni][nj] = next_id();
        }
    }

    int filled_pixels = 0;

    spread_ocean_tiles(base_map, sea_tile_map, false, false, filled_pixels);
    spread_ocean_tiles(base_map, sea_tile_map, false, true, filled_pixels);
    spread_ocean_tiles(base_map, sea_tile_map, true, false, filled_pixels);

    std::unordered_map<int, std::unordered_set<std::pair<int, int>, hash_coords> > tile_pixels;

    for (int i = 0; i < base_map.width(); i++) {
        for (int j = 0; j < base_map.height(); j++) {
            int id = sea_tile_map[i][j];
            tile_pixels[id].insert({i, j});
            if (id == 0) continue;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;
                    const int ni = i + dx;
                    const int nj = j + dy;
                    if (ni < 0 || ni >= base_map.width() || nj < 0 || nj >= base_map.height()) continue;
                    if (is_land(base_map(ni, nj))) {
                        break;
                    }
                }
            }
        }
    }
}

inline void write_sea_tiles(const image &province_map, const sea_tile_id_map &sea_tile_map) {
    std::unordered_set<unsigned int> used_colors;
    std::unordered_map<int, unsigned int> tile_colors;

    for (int i = 0; i < province_map.width(); i++) {
        for (int j = 0; j < province_map.height(); j++) {
            used_colors.insert(static_cast<unsigned int>(province_map(i, j)));
        }
    }

    std::uniform_int_distribution<unsigned int> distribution(0, 255 * 256 * 256 - 1);

    const int max_id = next_id();
    for (int i = 1; i < max_id; i++) {
        unsigned int color = distribution(generator);
        while (used_colors.contains(color)) {
            color = distribution(generator);
        }
        tile_colors[i] = color;
        used_colors.insert(color);
    }

    for (int i = 0; i < province_map.width(); i++) {
        for (int j = 0; j < province_map.height(); j++) {
            if (sea_tile_map[i][j] == 0) {
                continue;
            }
            if (static_cast<unsigned int>(province_map(i, j)) != 0xFFFFFF) {
                continue;
            }
            int id = sea_tile_map[i][j];
            province_map(i, j) = tile_colors[id];
        }
    }

    province_map.write();
}

inline void generate_tiles() {
    const image base_map("images/map.png");
    const image province_map("images/write_map.png");
    auto id_map = std::vector(static_cast<size_t>(base_map.width()),
                              std::vector(static_cast<size_t>(base_map.height()), 0));
    generate_lakes(base_map, id_map);
    generate_ocean_tiles(base_map, id_map);
    write_sea_tiles(province_map, id_map);
}
