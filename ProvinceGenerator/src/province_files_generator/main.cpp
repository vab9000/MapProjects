#include <unordered_map>
#include <vector>
#include <ranges>
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
                !provinces.contains(color) && color != 0xFFFFFF) {
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
            if (const auto color = static_cast<unsigned int>(base_map(i, j)); provinces.contains(color)) {
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

    for (int i = 0; i < base_map.width(); i++) {
        for (int j = 0; j < base_map.height(); j++) {
            if (const auto color = static_cast<unsigned int>(base_map(i, j)); provinces.contains(color)) {
                const auto elevation_color = elevation_image(i, j);
                if (static_cast<unsigned int>(elevation_color) == 0) {
                    continue;
                }
                elevation_data[color].push_back(static_cast<unsigned int>(elevation_color));
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
            if (const auto color = static_cast<unsigned int>(base_map(i, j)); provinces.contains(color)) {
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
            if (const auto color = static_cast<unsigned int>(base_map(i, j)); provinces.contains(color)) {
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

void generate_province_files() {
    const image province_image("images/write_map.png");

    std::unordered_map<unsigned int, province> provinces;

    load_provinces(province_image, provinces);

    load_koppen(province_image, provinces, "images/koppen.png");
    load_elevation(province_image, provinces, "images/elevation.png");
    load_vegetation(province_image, provinces, "images/vegetation.png");
    load_soil(province_image, provinces, "images/soil.png");

    save_provinces(provinces);

    // draw_map(province_image, provinces, "koppen_generated.png", &province::koppen_color);
    // draw_map(province_image, provinces, "elevation_generated.png", &province::elevation_color);
    // draw_map(province_image, provinces, "vegetation_generated.png", &province::vegetation_color);
    draw_map(province_image, provinces, "soil_generated.png", &province::soil_color);
}

int main() {
    generate_province_files();
}
