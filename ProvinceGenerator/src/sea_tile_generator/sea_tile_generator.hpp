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

constexpr unsigned int to_integer_color(const unsigned char r, const unsigned char g, const unsigned char b) {
    return (static_cast<unsigned int>(r) << 16) | (static_cast<unsigned int>(g) << 8) | static_cast<unsigned int>(b);
}

enum class sea_tile_t :unsigned int {
    coast = to_integer_color(25, 255, 255),
    sea = to_integer_color(100, 200, 255),
    southeasterly = to_integer_color(255, 100, 100),
    northeasterly = to_integer_color(100, 100, 255),
    westerly = to_integer_color(255, 255, 100),
    polar = to_integer_color(200, 255, 255),
    lake = to_integer_color(255, 150, 255),
};

using tiles = std::unordered_map<int, sea_tile_t>;
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
    tiles &t, const image &base_map, sea_tile_id_map &sea_tile_map, const int x, const int y) {
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

inline void generate_lakes(tiles &t, const image &base_map, sea_tile_id_map &sea_tile_map) {
    for (int i = 0; i < base_map.width(); i++) {
        for (int j = 0; j < base_map.height(); j++) {
            if (is_lake(base_map(i, j)) && sea_tile_map[i][j] == 0) {
                const auto lake_tiles = find_entire_lake(t, base_map, sea_tile_map, i, j);
                if (lake_tiles.empty()) continue;
                const int id = next_id();
                t[id] = sea_tile_t::lake;
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

inline std::vector<std::unordered_set<int> > generate_neighbors(const image &base_map,
                                                                const sea_tile_id_map &sea_tile_map) {
    std::unordered_map<int, std::unordered_set<int> > neighbors;
    for (int i = 0; i < base_map.width(); i++) {
        for (int j = 0; j < base_map.height(); j++) {
            if (is_land(base_map(i, j))) continue;
            auto this_tile_id = sea_tile_map[i][j];
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;
                    const int ni = i + dx;
                    const int nj = j + dy;
                    if (ni < 0 || ni >= base_map.width() || nj < 0 || nj >= base_map.height()) continue;
                    if (is_land(base_map(ni, nj))) continue;
                    int id = sea_tile_map[ni][nj];
                    neighbors[this_tile_id].insert(id);
                }
            }
        }
    }
    std::vector<std::unordered_set<int> > clusters;
    while (!neighbors.empty()) {
        const auto tile_id = neighbors.begin()->first;
        std::unordered_set<int> cluster;
        std::queue<int> queue;
        queue.push(tile_id);
        while (!queue.empty()) {
            int current_tile = queue.front();
            queue.pop();
            cluster.insert(current_tile);
            for (const auto &tile_neighbors = neighbors.at(current_tile); const auto &neighbor_id: tile_neighbors) {
                if (cluster.insert(neighbor_id).second) {
                    queue.push(neighbor_id);
                }
            }
            neighbors.erase(current_tile);
        }
        clusters.push_back(std::move(cluster));
    }
    return clusters;
}

inline void generate_ocean_tiles(const image &base_map, sea_tile_id_map &sea_tile_map, tiles &t) {
    constexpr int tile_size = 50;
    std::uniform_int_distribution dis(-tile_size / 3, tile_size / 3);

    for (int i = tile_size / 2; i < base_map.width(); i += tile_size) {
        for (int j = tile_size / 2; j < base_map.height(); j += tile_size) {
            sea_tile_map[i + dis(generator)][j + dis(generator)] = next_id();
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
            if (!t.contains(id)) {
                tile_pixels[id].insert({i, j});
            }
            if (id == 0) continue;
            bool borders_land = false;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;
                    const int ni = i + dx;
                    const int nj = j + dy;
                    if (ni < 0 || ni >= base_map.width() || nj < 0 || nj >= base_map.height()) continue;
                    if (is_land(base_map(ni, nj))) {
                        borders_land = true;
                        break;
                    }
                }
                if (borders_land) break;
            }
            if (borders_land) {
                t[id] = sea_tile_t::coast;
            }
        }
    }

    auto neighbors = generate_neighbors(base_map, sea_tile_map);

    size_t largest_cluster_size = 0;
    int largest_cluster_index = 0;
    for (int i = 0; i < neighbors.size(); i++) {
        if (neighbors[i].size() > largest_cluster_size) {
            largest_cluster_size = neighbors[i].size();
            largest_cluster_index = i;
        }
    }
    for (int i = 0; const auto &cluster: neighbors) {
        if (i == largest_cluster_index) {
            ++i;
            continue;
        }
        for (const auto &id: cluster) {
            if (!t.contains(id)) {
                t[id] = sea_tile_t::sea;
            }
        }
        ++i;
    }

    neighbors.clear();

    for (auto &[id, pixels]: tile_pixels) {
        if (t.contains(id)) continue;
        if (pixels.empty()) continue;

        const int average_y = static_cast<int>(std::accumulate(pixels.begin(), pixels.end(), 0,
                                                               [](const int sum, const std::pair<int, int> &p) {
                                                                   return sum + p.second;
                                                               }) / pixels.size());

        t[id] = average_y < base_map.height() / 180 * 30
                    ? t[id] = sea_tile_t::polar
                    : average_y < base_map.height() / 180 * 60
                          ? t[id] = sea_tile_t::westerly
                          : average_y < base_map.height() / 180 * 90
                                ? t[id] = sea_tile_t::northeasterly
                                : average_y < base_map.height() / 180 * 120
                                      ? t[id] = sea_tile_t::southeasterly
                                      : average_y < base_map.height() / 180 * 150
                                            ? t[id] = sea_tile_t::westerly
                                            : t[id] = sea_tile_t::polar;

        pixels.clear();
    }

    tile_pixels.clear();
}

inline void write_sea_tiles(const image &province_map, const tiles &t, const sea_tile_id_map &sea_tile_map) {
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

inline void write_sea_tile_file(const tiles &t, const std::string &filename) {
    std::ofstream file(filename);
    for (const auto &[id, tile_type]: t) {
        file << "ID: " << id << '\n';
        file << "Type: " << static_cast<unsigned int>(tile_type) << '\n';
    }
    file.close();
}

inline void generate_tiles() {
    const image base_map("images/map.png");
    const image province_map("images/write_map.png");
    auto id_map = std::vector(static_cast<size_t>(base_map.width()),
                              std::vector(static_cast<size_t>(base_map.height()), 0));
    tiles t;
    generate_lakes(t, base_map, id_map);
    generate_ocean_tiles(base_map, id_map, t);
    write_sea_tiles(province_map, t, id_map);
    write_sea_tile_file(t, "sea_tiles.txt");
}
