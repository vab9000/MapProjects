#pragma once

// This exists because I goofed

#include <fstream>
#include <unordered_map>
#include "../common/image.hpp"

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

inline std::vector<std::unordered_set<int> > generate_neighbors(const image &base_map,
                                                                const sea_tile_id_map &sea_tile_map,
                                                                const std::unordered_map<unsigned int, sea_tile_t> &
                                                                types) {
    std::unordered_map<int, std::unordered_set<int> > neighbors;
    for (int i = 0; i < base_map.width(); i++) {
        for (int j = 0; j < base_map.height(); j++) {
            if (is_land(base_map(i, j))) continue;
            auto this_tile_id = sea_tile_map[i][j];
            if (types.contains(this_tile_id)) continue;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;
                    const int ni = i + dx < 0 ? base_map.width() - 1 : (i + dx) % base_map.width();
                    const int nj = j + dy;
                    if (nj < 0 || nj >= base_map.height()) continue;
                    if (is_land(base_map(ni, nj))) continue;
                    int id = sea_tile_map[ni][nj];
                    if (types.contains(id)) continue;
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

inline std::unordered_map<unsigned int, sea_tile_t> gen_types(const image &map_image, const image &base_image) {
    std::unordered_map<unsigned int, sea_tile_t> types;
    std::unordered_map<unsigned int, std::vector<std::pair<int, int> > > pixels;

    std::vector sea_tile_map(base_image.width(), std::vector(base_image.height(), 0));

    for (int i = 0; i < map_image.width(); i++) {
        for (int j = 0; j < map_image.height(); j++) {
            const auto color = static_cast<unsigned int>(map_image(i, j));
            if (is_lake(base_image(i, j))) {
                types[color] = sea_tile_t::lake;
            }
            sea_tile_map[i][j] = static_cast<int>(color);
            if (types.contains(color)) continue;
            if (is_land(base_image(i, j))) continue;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    const int nx = i + dx, ny = j + dy;
                    if (nx < 0 || nx >= map_image.width() || ny < 0 || ny >= map_image.height()) continue;
                    if (is_land(base_image(nx, ny))) {
                        types[color] = sea_tile_t::coast;
                    }
                }
            }
            pixels[color].emplace_back(i, j);
        }
    }

    const auto neighbors = generate_neighbors(base_image, sea_tile_map, types);

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
            if (!types.contains(static_cast<unsigned int>(id))) {
                types[static_cast<unsigned int>(id)] = sea_tile_t::sea;
            }
        }
        ++i;
    }

    for (const auto &[color, coords]: pixels) {
        if (types.contains(color)) continue;
        const int average_y = static_cast<int>(std::accumulate(coords.begin(), coords.end(), 0,
                                                               [](const int sum, const std::pair<int, int> &p) {
                                                                   return sum + p.second;
                                                               }) / coords.size());

        types[color] = average_y < base_image.height() / 180 * 30
                           ? types[color] = sea_tile_t::polar
                           : average_y < base_image.height() / 180 * 60
                                 ? types[color] = sea_tile_t::westerly
                                 : average_y < base_image.height() / 180 * 90
                                       ? types[color] = sea_tile_t::northeasterly
                                       : average_y < base_image.height() / 180 * 120
                                             ? types[color] = sea_tile_t::southeasterly
                                             : average_y < base_image.height() / 180 * 150
                                                   ? types[color] = sea_tile_t::westerly
                                                   : types[color] = sea_tile_t::polar;
    }

    return types;
}

inline void fix_sea_tile_file() {
    const image map_image{"images/write_map.png"};
    const image base_image{"images/map.png"};

    const auto types = gen_types(map_image, base_image);

    std::ofstream sea_file{"sea_tiles.txt"};

    for (const auto &[color, tile_type]: types) {
        sea_file << "Color: " << color << '\n';
        sea_file << "Type: " << static_cast<unsigned int>(tile_type) << '\n';
    }
    sea_file.close();
}
