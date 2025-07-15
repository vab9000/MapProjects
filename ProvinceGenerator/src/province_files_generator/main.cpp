#include <unordered_map>
#include <vector>
#include <ranges>
#include <fstream>
#include <random>
#include <functional>
#include <iostream>
#include <numeric>
#include "../common/image.hpp"
#include "province.hpp"

using provinces_map = std::unordered_map<unsigned int, province>;

inline image::image_color search_spiral(const image &search_image, const int x, const int y,
                                        const std::function<bool(image::image_color)> &valid_color,
                                        const int max_radius = 10) {
    int current_x = x, current_y = y - 1;
    int current_radius = 1;
    std::pair current_direction = {1, 1};
    while (current_radius < max_radius) {
        if (current_x >= 0 && current_x < search_image.width() && current_y >= 0 && current_y < search_image.height()) {
            if (const auto color = search_image(current_x, current_y); valid_color(color)) {
                return color;
            }
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
    return search_image(x, y);
}

inline void load_provinces(const std::string &base_map_path, const image &province_image, provinces_map &provinces) {
    const image base_map(base_map_path);

    for (int i = 0; i < province_image.width(); i++) {
        for (int j = 0; j < province_image.height(); j++) {
            if (const auto color = static_cast<unsigned int>(province_image(i, j));
                !provinces.contains(color)) {
                if (const auto base_color = static_cast<unsigned int>(base_map(i, j));
                    base_color == 0xFF00FF || base_color == 0x00FFFF) {
                    provinces[color] = province{color, true};
                } else {
                    provinces[color] = province{color, false};
                }
            }
        }
    }
}

inline void load_koppen(const image &base_map, provinces_map &provinces, const std::string &koppen_file_path) {
    const image koppen_image(koppen_file_path);
    std::unordered_map<unsigned int, std::vector<unsigned int> > koppen_data;

    constexpr auto valid_koppen_color = [](const image::image_color color) {
        return static_cast<unsigned int>(color) != 0;
    };

    for (int i = 0; i < base_map.width(); i++) {
        for (int j = 0; j < base_map.height(); j++) {
            if (const auto color = static_cast<unsigned int>(base_map(i, j)); !provinces.at(color).is_water()) {
                if (const auto koppen_color = koppen_image(i, j); !valid_koppen_color(
                    koppen_color)) {
                    if (const auto new_color = search_spiral(koppen_image, i, j, valid_koppen_color);
                        valid_koppen_color(new_color)) {
                        koppen_data[color].push_back(static_cast<unsigned int>(new_color));
                    }
                } else {
                    koppen_data[color].push_back(static_cast<unsigned int>(koppen_color));
                }
            }
        }
    }
    for (auto &[color, province]: provinces) {
        if (province.is_water()) continue;
        province.set_koppen(koppen_data[color]);
    }
}

struct province_pair_hash {
    std::size_t operator()(const std::pair<province *, province *> &p) const noexcept {
        return std::hash<province *>()(p.first) ^ (std::hash<province *>()(p.second));
    }
};

inline void load_elevation(const image &base_map, provinces_map &provinces, const std::string &elevation_file_path) {
    const image elevation_image(elevation_file_path);
    std::unordered_map<unsigned int, std::vector<unsigned int> > elevation_data;

    constexpr auto valid_elevation_color = [](const image::image_color color) {
        return static_cast<unsigned int>(color) != 0;
    };

    for (int i = 0; i < base_map.width(); i++) {
        for (int j = 0; j < base_map.height(); j++) {
            if (const auto color = static_cast<unsigned int>(base_map(i, j)); !provinces.at(color).is_water()) {
                if (const auto elevation_color = elevation_image(i, j); !valid_elevation_color(
                    elevation_color)) {
                    const auto new_color = search_spiral(elevation_image, i, j, valid_elevation_color, 3);
                    elevation_data[color].push_back(static_cast<unsigned int>(new_color));
                } else {
                    elevation_data[color].push_back(static_cast<unsigned int>(elevation_color));
                }
            }
        }
    }
    for (auto &[color, province]: provinces) {
        if (province.is_water()) continue;
        province.set_elevation(elevation_data[color]);
    }
}

inline void draw_impassable_map(
    const std::unordered_set<std::pair<province *, province *>, province_pair_hash> &impassable_neighbors,
    const int width,
    const int height, const std::string &output_path) {
    const image impassable_map(output_path, width, height);
    for (int i = 0; i < impassable_map.width(); i++) {
        for (int j = 0; j < impassable_map.height(); j++) {
            impassable_map(i, j) = 0xFFFFFF;
        }
    }
    for (const auto &[p, neighbor]: impassable_neighbors) {
        if (p->color() > neighbor->color()) continue;
        for (const auto &[x, y]: p->outline().at(neighbor)) {
            impassable_map(x, y) = 0xFF0000;
        }
        for (const auto &[x, y]: neighbor->outline().at(p)) {
            impassable_map(x, y) = 0xFF0000;
        }
    }
    impassable_map.write();
}

inline void generate_impassable_crossings(const image &base_map, provinces_map &provinces,
                                          const std::string &output_path) {
    std::unordered_set<std::pair<province *, province *>, province_pair_hash> impassable_neighbors;
    for (auto &p: provinces | std::views::values) {
        if (p.is_water()) continue;
        for (const auto impassable_provinces = p.impassable_neighbors(base_map); const auto &neighbor:
             impassable_provinces) {
            impassable_neighbors.insert({&p, const_cast<province *>(neighbor)});
        }
    }
    std::ofstream file{output_path};
    for (const auto &[province, neighbor]: impassable_neighbors) {
        if (province->color() > neighbor->color()) continue;
        file << "Province Color: " << province->color() << "\n";
        file << "Neighbor Color: " << neighbor->color() << "\n";
    }
    file.close();
    draw_impassable_map(impassable_neighbors, base_map.width(), base_map.height(), "impassable_crossings.png");
}

inline void load_vegetation(const image &base_map, provinces_map &provinces, const std::string &vegetation_file_path) {
    const image vegetation_image(vegetation_file_path);
    std::unordered_map<unsigned int, std::vector<unsigned int> > vegetation_data;

    constexpr auto valid_vegetation_color = [](const image::image_color color) {
        return static_cast<unsigned int>(color) != 0;
    };

    for (int i = 0; i < base_map.width(); i++) {
        for (int j = 0; j < base_map.height(); j++) {
            if (const auto color = static_cast<unsigned int>(base_map(i, j)); !provinces.at(color).is_water()) {
                if (const auto vegetation_color = vegetation_image(i, j); !valid_vegetation_color(
                    vegetation_color)) {
                    const auto new_color = search_spiral(vegetation_image, i, j, valid_vegetation_color, 3);
                    vegetation_data[color].push_back(static_cast<unsigned int>(new_color));
                } else {
                    vegetation_data[color].push_back(static_cast<unsigned int>(vegetation_color));
                }
            }
        }
    }
    for (auto &[color, province]: provinces) {
        if (province.is_water()) continue;
        province.set_vegetation(vegetation_data[color]);
    }
}

inline void load_soil(const image &base_map, provinces_map &provinces, const std::string &soil_file_path) {
    const image soil_image(soil_file_path);
    std::unordered_map<unsigned int, std::vector<unsigned int> > soil_data;

    constexpr auto valid_soil_color = [](const image::image_color color) {
        return static_cast<unsigned int>(color) != 0;
    };

    for (int i = 0; i < base_map.width(); i++) {
        for (int j = 0; j < base_map.height(); j++) {
            if (const auto color = static_cast<unsigned int>(base_map(i, j)); !provinces.at(color).is_water()) {
                if (const auto soil_color = soil_image(i, j); !valid_soil_color(
                    soil_color)) {
                    const auto new_color = search_spiral(soil_image, i, j, valid_soil_color, 3);
                    soil_data[color].push_back(static_cast<unsigned int>(new_color));
                } else {
                    soil_data[color].push_back(static_cast<unsigned int>(soil_color));
                }
            }
        }
    }
    for (auto &[color, province]: provinces) {
        if (province.is_water()) continue;
        province.set_soil(soil_data[color]);
    }
}

inline void save_provinces(const provinces_map &provinces) {
    std::ofstream file{"provinces.txt"};
    for (const auto &province: provinces | std::views::values) {
        province.write(file);
    }
    file.close();
}

inline void generate_neighbors(const image &base_map, provinces_map &provinces) {
    for (int i = 0; i < base_map.width(); i++) {
        for (int j = 0; j < base_map.height(); j++) {
            if (const auto color = static_cast<unsigned int>(base_map(i, j)); provinces.contains(color)) {
                if (provinces.at(color).is_water()) continue;
                auto &province = provinces.at(color);
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        if ((dx == 0 && dy == 0) || (dx != 0 && dy != 0)) continue;
                        if (const int nx = i + dx, ny = j + dy;
                            nx >= 0 && nx < base_map.width() && ny >= 0 && ny < base_map.height()) {
                            if (const auto neighbor_color = static_cast<unsigned int>(base_map(nx, ny));
                                neighbor_color != color) {
                                if (provinces.at(neighbor_color).is_water()) continue;
                                province.add_neighbor(&provinces.at(neighbor_color));
                                province.add_outline_point(&provinces.at(neighbor_color), i, j);
                            }
                        }
                    }
                }
            }
        }
    }
}

inline std::unordered_set<std::pair<int, int>, hash_coords> find_river(
    const image &base_image, const image &river_image,
    const province *const search_province,
    const province *const neighbor) {
    constexpr auto valid_river_color = [](const image::image_color color) {
        return color.b() != 0 && color.r() == 0 && color.g() == 0;
    };

    std::unordered_set<std::pair<int, int>, hash_coords> data;

    std::function<void(int, int)> search_neighbors = [&](const int x, const int y) {
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if ((dx == 0 && dy == 0) || (dx != 0 && dy != 0)) continue;

                if (const int nx = x + dx, ny = y + dy;
                    nx >= 0 && nx < river_image.width() && ny >= 0 && ny < river_image.height()) {
                    if (const auto base_color = static_cast<unsigned int>(base_image(nx, ny));
                        valid_river_color(river_image(nx, ny)) && (
                            base_color == search_province->color() || base_color == neighbor->color())) {
                        if (data.insert({nx, ny}).second) {
                            search_neighbors(nx, ny);
                        }
                    }
                }
            }
        }
    };

    for (const auto &[x, y]: search_province->outline().at(const_cast<province * const>(neighbor))) {
        if (valid_river_color(river_image(x, y))) {
            if (data.insert({x, y}).second) {
                search_neighbors(x, y);
            }
        }
    }

    for (const auto &[x, y]: neighbor->outline().at(const_cast<province * const>(search_province))) {
        if (valid_river_color(river_image(x, y))) {
            if (data.insert({x, y}).second) {
                search_neighbors(x, y);
            }
        }
    }

    return data;
}

inline bool river_between(const image &river_map, const province &self, const province &neighbor) {
    constexpr auto valid_river_color = [](const image::image_color color) {
        return color.b() != 0 && color.r() == 0 && color.g() == 0;
    };
    int total_outline = 0;
    int river_outline = 0;
    for (const auto &[x, y]: self.outline().at(const_cast<province *>(&neighbor))) {
        if (valid_river_color(river_map(x, y))) {
            river_outline++;
        }
        total_outline++;
    }
    for (const auto &[x, y]: neighbor.outline().at(const_cast<province *>(&self))) {
        if (valid_river_color(river_map(x, y))) {
            river_outline++;
        }
        total_outline++;
    }
    return total_outline * 0.3 < river_outline;
}

inline void generate_rivers(image &base_map, provinces_map &provinces, const std::string &river_file_path,
                            const std::string &river_tile_file_path) {
    const image river_image(river_file_path);

    std::unordered_map<std::pair<province *, province *>, std::unordered_set<std::pair<int, int>, hash_coords>,
        province_pair_hash> rivers_data;

    for (auto &p: provinces | std::views::values) {
        if (p.is_water()) continue;
        for (const auto &neighbor: p.neighbors()) {
            if (neighbor->is_water()) continue;
            if (!river_between(river_image, p, *neighbor)) continue;
            if (p.color() < neighbor->color()) {
                if (auto river_points = find_river(base_map, river_image, &p, neighbor); !river_points.
                    empty()) {
                    rivers_data[{&p, const_cast<province *>(neighbor)}] = std::move(river_points);
                }
            }
        }
    }

    std::unordered_set<unsigned int> used_colors;
    for (int i = 0; i < base_map.width(); i++) {
        for (int j = 0; j < base_map.height(); j++) {
            used_colors.insert(static_cast<unsigned int>(base_map(i, j)));
        }
    }
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<unsigned int> color_distribution(0x000000, 0xFFFFFE);

    std::vector<std::pair<unsigned int, unsigned int> > river_colors;

    for (const auto &[pair, river_points]: rivers_data) {
        auto &p = *pair.first;
        auto &neighbor = *pair.second;

        int river_size = 0;
        river_size = static_cast<int>(std::accumulate(river_points.begin(), river_points.end(), 0,
                                                      [&river_image](const int sum, const std::pair<int, int> &point) {
                                                          return sum + river_image(point.first, point.second).b();
                                                      }) / river_points.size());

        bool river_placed = false;
        auto new_color = color_distribution(generator);
        while (used_colors.contains(new_color)) {
            new_color = color_distribution(generator);
        }
        for (const auto &[x, y]: river_points) {
            if (static_cast<unsigned int>(base_map(x, y)) != p.color() &&
                static_cast<unsigned int>(base_map(x, y)) != neighbor.color()) {
                continue;
            }
            base_map(x, y) = new_color;
            river_placed = true;
        }
        if (river_placed) {
            used_colors.insert(new_color);
            river_colors.emplace_back(new_color, river_size);
            provinces.emplace(new_color, province{new_color, true});
        }

        if (river_points.size() > 4) {
            p.add_river(&neighbor, river_size);
            neighbor.add_river(&p, river_size);
        }
    }

    std::ofstream river_file{river_tile_file_path};
    for (const auto [color, size]: river_colors) {
        river_file << "River Color: " << color << "\n";
        river_file << "River Size: " << size << "\n";
    }
    river_file.close();
    base_map.write();
}

inline void write_rivers(const provinces_map &provinces, const std::string &output_path) {
    std::ofstream file(output_path);
    for (const auto &province: provinces | std::views::values) {
        for (const auto &[neighbor, size]: province.rivers()) {
            if (province.color() > neighbor->color()) continue;
            file << "Province Color: " << province.color() << "\n";
            file << "Neighbor Color: " << neighbor->color() << "\n";
            file << "River Size: " << size << "\n";
        }
    }
    file.close();
}

inline void draw_river_map(const image &base_map, const provinces_map &provinces,
                           const std::string &output_path) {
    const image write_map(output_path, base_map.width(), base_map.height());

    for (int i = 0; i < base_map.width(); i++) {
        for (int j = 0; j < base_map.height(); j++) {
            const auto color = static_cast<unsigned int>(base_map(i, j));
            write_map(i, j) = provinces.at(color).river_color(i, j);
        }
    }

    write_map.write();
}

inline std::unordered_set<std::pair<int, int>, hash_coords> find_entire_river(const image &base_map, int x, int y) {
    std::unordered_set<std::pair<int, int>, hash_coords> river_points;
    river_points.insert({x, y});

    const std::function valid_river_color = [](const image::image_color color) {
        return color.b() != 0 && color.r() == 0 && color.g() == 0;
    };

    const std::function<void(int, int)> search_neighbors = [&](const int cx, const int cy) {
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;

                if (int nx = cx + dx, ny = cy + dy;
                    nx >= 0 && nx < base_map.width() && ny >= 0 && ny < base_map.height()) {
                    if (const auto color = base_map(nx, ny);
                        river_points.insert({nx, ny}).second && valid_river_color(color)) {
                        search_neighbors(nx, ny);
                    }
                }
            }
        }
    };

    search_neighbors(x, y);

    return river_points;
}

inline void draw_map(const image &base_map, const provinces_map &provinces, const std::string &output_path,
                     unsigned int (province::*color_func)() const) {
    const image write_map(output_path, base_map.width(), base_map.height());

    for (int i = 0; i < base_map.width(); i++) {
        for (int j = 0; j < base_map.height(); j++) {
            if (const auto color = static_cast<unsigned int>(base_map(i, j)); provinces.contains(color)) {
                const auto &province = provinces.at(color);
                write_map(i, j) = (province.*color_func)();
            } else {
                write_map(i, j) = 0xFFFFFF;
            }
        }
    }

    write_map.write();
}

inline void generate_province_files() {
    image province_image("images/write_map.png");

    provinces_map provinces;

    load_provinces("images/map.png", province_image, provinces);
    std::cout << "Loaded " << provinces.size() << " provinces." << std::endl;

    generate_neighbors(province_image, provinces);
    std::cout << "Generated neighbors for provinces." << std::endl;

    load_koppen(province_image, provinces, "images/koppen.png");
    std::cout << "Loaded Koppen climate data." << std::endl;
    load_elevation(province_image, provinces, "images/elevation.png");
    std::cout << "Loaded elevation data." << std::endl;
    load_vegetation(province_image, provinces, "images/vegetation.png");
    std::cout << "Loaded vegetation data." << std::endl;
    load_soil(province_image, provinces, "images/soil.png");
    std::cout << "Loaded soil data." << std::endl;

    draw_map(province_image, provinces, "koppen_generated.png", &province::koppen_color);
    std::cout << "Generated Koppen map." << std::endl;
    draw_map(province_image, provinces, "elevation_generated.png", &province::elevation_color);
    std::cout << "Generated elevation map." << std::endl;
    draw_map(province_image, provinces, "vegetation_generated.png", &province::vegetation_color);
    std::cout << "Generated vegetation map." << std::endl;
    draw_map(province_image, provinces, "soil_generated.png", &province::soil_color);
    std::cout << "Generated soil map." << std::endl;

    generate_impassable_crossings(image{"images/map.png"}, provinces, "impassable_crossings.txt");
    std::cout << "Generated impassable crossings." << std::endl;

    save_provinces(provinces);
    std::cout << "Saved provinces data." << std::endl;

    generate_rivers(province_image, provinces, "images/map.png", "river_tiles.txt");
    std::cout << "Generated rivers data." << std::endl;

    draw_river_map(province_image, provinces, "rivers_generated.png");
    std::cout << "Generated river map." << std::endl;

    write_rivers(provinces, "rivers.txt");
    std::cout << "Saved rivers data." << std::endl;
}

int main() {
    generate_province_files();
}
