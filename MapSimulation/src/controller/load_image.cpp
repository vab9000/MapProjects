#include "load_image.hpp"

#include <execution>
#include "image.hpp"
#include "simulation.hpp"
#include "../model/base/province.hpp"
#include "../model/tags/country.hpp"
#include "data.hpp"

void load_image(data &data, image &map_image, double* progress) {
    auto process_pixel = [&](const pixel_t &pixel, const int *position) {
        const auto i = position[1];
        const auto j = position[0];
        if (const auto color = static_cast<unsigned int>(pixel.x) << 16 | static_cast<unsigned int>(pixel.y) << 8 |
               static_cast<unsigned int>(pixel.z); data.provinces.contains(color)) {
            const auto province = data.provinces.at(color).get();
            province->expand_bounds(i, j);
        } else {
            auto new_country = std::make_unique<country>("", color);
            auto new_province = std::make_unique<province>(
                std::string("Province ") + std::to_string(data.provinces.size()),
                color, i, j);
            new_province->set_owner(*new_country);
            data.tags.insert(std::make_pair(color, std::move(new_country)));
            data.provinces.insert(std::make_pair(color, std::move(new_province)));
        }
    };

    auto is_border = [&](const int x, const int y, const unsigned int color, const province *&other_province) {
        if (x >= 0 && x < map_image.width && y >= 0 && y < map_image.height && map_image.get_color(x, y) !=
            color) {
            other_province = data.provinces.at(map_image.get_color(x, y)).get();
            return true;
        }
        if (x == -1 && y >= 0 && y < map_image.height && map_image.get_color(map_image.width - 1, y) !=
            color) {
            other_province = data.provinces.at(map_image.get_color(map_image.width - 1, y)).get();
            return true;
        }
        if (x == map_image.width && y >= 0 && y < map_image.height && map_image.get_color(0, y) != color) {
            other_province = data.provinces.at(map_image.get_color(0, y)).get();
            return true;
        }
        return false;
    };

    auto process_pixel_borders = [&](const pixel_t &pixel, const int *position) {
        const auto i = position[1];
        const auto j = position[0];
        const auto color = static_cast<unsigned int>(pixel.x) << 16 | static_cast<unsigned int>(pixel.y) << 8 |
               static_cast<unsigned int>(pixel.z);
        province *province_at_pos = data.provinces.at(color).get();

        if (const province *other_province = nullptr;
            is_border(i - 1, j, color, other_province) || is_border(i + 1, j, color, other_province) ||
            is_border(i, j - 1, color, other_province) || is_border(i, j + 1, color, other_province)) {
            province_at_pos->add_outline(i, j, *other_province);
        } else {
            province_at_pos->add_pixel(i, j);
        }
    };

    map_image = image("assets/provinces.png");

    for (int i = 0; i < map_image.cv_image.rows; ++i) {
        for (int j = 0; j < map_image.cv_image.cols; ++j) {
            const int coords[2] = {i, j};
            process_pixel(map_image.cv_image.at<pixel_t>(i, j), coords);
        }
        *progress = static_cast<double>(i) / map_image.cv_image.rows / 2;
    }

    for (int i = 0; i < map_image.cv_image.rows; ++i) {
        for (int j = 0; j < map_image.cv_image.cols; ++j) {
            const int coords[2] = {i, j};
            process_pixel_borders(map_image.cv_image.at<pixel_t>(i, j), coords);
        }
        *progress = static_cast<double>(i) / map_image.cv_image.rows / 2 + 0.5;
    }

    auto province_values = data.provinces | std::views::values;

    std::for_each(std::execution::par, province_values.begin(), province_values.end(),
                  [](const std::unique_ptr<province> &province) { province->finalize(); });

    std::for_each(std::execution::par, province_values.begin(), province_values.end(),
                  [](const std::unique_ptr<province> &province) { province->process_distances(); });
}
