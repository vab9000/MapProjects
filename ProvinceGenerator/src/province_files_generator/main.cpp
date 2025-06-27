#include <unordered_map>
#include <vector>
#include <ranges>
#include <functional>
#include <numeric>
#include "../common/image.hpp"
#include "province.hpp"

using provinces_map = std::unordered_map<unsigned int, province>;

inline image::image_color search_spiral(const image &search_image, const int x, const int y,
                                        bool (*valid_color)(image::image_color), const int max_radius = 10) {
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

inline void load_provinces(const image &province_image, provinces_map &provinces) {
    for (int i = 0; i < province_image.width(); i++) {
        for (int j = 0; j < province_image.height(); j++) {
            if (const auto color = static_cast<unsigned int>(province_image(i, j));
                !provinces.contains(color)) {
                provinces[color] = province{color};
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
            if (const auto color = static_cast<unsigned int>(base_map(i, j)); color != 0xFFFFFF) {
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
        province.set_koppen(koppen_data[color]);
    }
}

inline void load_elevation(const image &base_map, provinces_map &provinces, const std::string &elevation_file_path) {
    const image elevation_image(elevation_file_path);
    std::unordered_map<unsigned int, std::vector<unsigned int> > elevation_data;

    constexpr auto valid_elevation_color = [](const image::image_color color) {
        return static_cast<unsigned int>(color) != 0;
    };

    for (int i = 0; i < base_map.width(); i++) {
        for (int j = 0; j < base_map.height(); j++) {
            if (const auto color = static_cast<unsigned int>(base_map(i, j)); color != 0xFFFFFF) {
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
        province.set_elevation(elevation_data[color]);
    }
}

inline void load_vegetation(const image &base_map, provinces_map &provinces, const std::string &vegetation_file_path) {
    const image vegetation_image(vegetation_file_path);
    std::unordered_map<unsigned int, std::vector<unsigned int> > vegetation_data;

    constexpr auto valid_vegetation_color = [](const image::image_color color) {
        return static_cast<unsigned int>(color) != 0;
    };

    for (int i = 0; i < base_map.width(); i++) {
        for (int j = 0; j < base_map.height(); j++) {
            if (const auto color = static_cast<unsigned int>(base_map(i, j)); color != 0xFFFFFF) {
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
            if (const auto color = static_cast<unsigned int>(base_map(i, j)); color != 0xFFFFFF) {
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
        province.set_soil(soil_data[color]);
    }
}

inline void save_provinces(const provinces_map &provinces) {
    std::ofstream file{"provinces.txt"};
    for (const auto province: provinces | std::views::values) {
        province.write(file);
    }
    file.close();
}

inline void generate_neighbors(const image &base_map, provinces_map &provinces) {
    for (int i = 0; i < base_map.width(); i++) {
        for (int j = 0; j < base_map.height(); j++) {
            if (const auto color = static_cast<unsigned int>(base_map(i, j)); provinces.contains(color)) {
                if (color == 0xFFFFFF) continue;
                auto &province = provinces.at(color);
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        if ((dx == 0 && dy == 0) || (dx != 0 && dy != 0)) continue;
                        if (const int nx = i + dx, ny = j + dy;
                            nx >= 0 && nx < base_map.width() && ny >= 0 && ny < base_map.height()) {
                            if (const auto neighbor_color = static_cast<unsigned int>(base_map(nx, ny));
                                neighbor_color != color) {
                                if (neighbor_color == 0xFFFFFF) continue;
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

struct river_hash {
    std::size_t operator()(const std::pair<province *, province *> &p) const noexcept {
        return std::hash<province *>()(p.first) ^ (std::hash<province *>()(p.second));
    }
};

inline std::unordered_set<std::pair<int, int>, hash_coords> find_river(
    const image &base_image, const image &river_image,
    province &search_province,
    province &neighbor) {
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
                            base_color == search_province.color() || base_color == neighbor.color())) {
                        if (data.insert({nx, ny}).second) {
                            search_neighbors(nx, ny);
                        }
                    }
                }
            }
        }
    };

    for (const auto &[x, y]: search_province.outline().at(&neighbor)) {
        if (valid_river_color(river_image(x, y))) {
            if (data.insert({x, y}).second) {
                search_neighbors(x, y);
            }
        }
    }

    for (const auto &[x, y]: neighbor.outline().at(&search_province)) {
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

inline void generate_rivers(const image &base_map, provinces_map &provinces, const std::string &river_file_path) {
    generate_neighbors(base_map, provinces);

    const image river_image(river_file_path);

    std::unordered_map<std::pair<province *, province *>, std::unordered_set<std::pair<int, int>, hash_coords>,
        river_hash> rivers_data;

    for (auto &province: provinces | std::views::values) {
        if (province.color() == 0xFFFFFF) continue;
        for (const auto &neighbor: province.neighbors()) {
            if (neighbor->color() == 0xFFFFFF) continue;
            if (!river_between(river_image, province, *neighbor)) continue;
            if (province.color() < neighbor->color()) {
                if (const auto river_points = find_river(base_map, river_image, province, *neighbor); !river_points.
                    empty()) {
                    rivers_data[{&province, neighbor}] = std::move(river_points);
                }
            }
        }
    }

    for (const auto &[pair, river_points]: rivers_data) {
        auto &province = *pair.first;
        auto &neighbor = *pair.second;

        int river_size = 0;
        river_size = std::accumulate(river_points.begin(), river_points.end(), 0,
                                     [&river_image](const int sum, const std::pair<int, int> &point) {
                                         return sum + river_image(point.first, point.second).b();
                                     }) / river_points.size();

        province.add_river(&neighbor, river_size);
        neighbor.add_river(&province, river_size);
    }
}

inline void write_rivers(const provinces_map &provinces, const std::string &output_path) {
    std::ofstream file(output_path);
    for (const auto &province: provinces | std::views::values) {
        for (const auto &[neighbor, size]: province.rivers()) {
            if (province.color() > neighbor->color()) continue;
            file << "Province Color: " << province.color() << "\n";
            file << "Neighbor Color: " << neighbor->color() << "\nRiver Size: " << size << "\n";
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
    const image province_image("images/write_map.png");

    std::unordered_map<unsigned int, province> provinces;

    load_provinces(province_image, provinces);

    // load_koppen(province_image, provinces, "images/koppen.png");
    // load_elevation(province_image, provinces, "images/elevation.png");
    // load_vegetation(province_image, provinces, "images/vegetation.png");
    // load_soil(province_image, provinces, "images/soil.png");
    //
    // save_provinces(provinces);

    generate_rivers(province_image, provinces, "images/map.png");

    // draw_river_map(province_image, provinces, "rivers_generated.png");

    write_rivers(provinces, "rivers.txt");

    // draw_map(province_image, provinces, "koppen_generated.png", &province::koppen_color);
    // draw_map(province_image, provinces, "elevation_generated.png", &province::elevation_color);
    // draw_map(province_image, provinces, "vegetation_generated.png", &province::vegetation_color);
    // draw_map(province_image, provinces, "soil_generated.png", &province::soil_color);
}

int main() {
    generate_province_files();
}
