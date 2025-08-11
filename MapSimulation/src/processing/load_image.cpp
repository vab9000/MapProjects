#include "load_image.hpp"
#include <algorithm>
#include <execution>
#include <fstream>
#include <iostream>
#include <ranges>
#include "image.hpp"
#include "../mechanics/data.hpp"
#include "../utils/colors.hpp"

namespace {
    auto load_provinces(mechanics::data &d) -> void {
        std::ifstream province_file("assets/provinces.txt");
        if (!province_file.is_open()) { throw std::runtime_error("Failed to open province file: " "provinces.txt"); }
        while (!province_file.eof()) {
            std::string ignore;
            std::getline(province_file, ignore, ':');
            if (ignore == "\n") { break; }
            uint_fast32_t color{};
            province_file >> color;
            std::getline(province_file, ignore, ':');
            uint_fast32_t koppen_value{};
            province_file >> koppen_value;
            std::getline(province_file, ignore, ':');
            uint_fast32_t elevation_value{};
            province_file >> elevation_value;
            std::getline(province_file, ignore, ':');
            uint_fast32_t vegetation_value{};
            province_file >> vegetation_value;
            std::getline(province_file, ignore, ':');
            uint_fast32_t soil_value{};
            province_file >> soil_value;
            std::getline(province_file, ignore);
            color = utils::flip_rb(color);
            d.emplace_province(static_cast<mechanics::koppen_t>(utils::flip_rb(koppen_value)),
                static_cast<mechanics::elevation_t>(utils::flip_rb(elevation_value)),
                static_cast<mechanics::vegetation_t>(utils::flip_rb(vegetation_value)),
                static_cast<mechanics::soil_t>(utils::flip_rb(soil_value)),
                mechanics::sea_t::none, color);
        }
        province_file.close();
    }

    auto load_seas(mechanics::data &d) -> void {
        std::ifstream sea_file("assets/sea_tiles.txt");
        if (!sea_file.is_open()) { throw std::runtime_error("Failed to open sea tiles file: " "sea_tiles.txt"); }
        while (!sea_file.eof()) {
            std::string ignore;
            std::getline(sea_file, ignore, ':');
            if (ignore == "\n") { break; }
            uint_fast32_t color{};
            sea_file >> color;
            std::getline(sea_file, ignore, ':');
            uint_fast32_t sea_value{};
            sea_file >> sea_value;
            color = utils::flip_rb(color);
            d.emplace_province(mechanics::koppen_t::none, mechanics::elevation_t::none,
                mechanics::vegetation_t::none, mechanics::soil_t::none,
                static_cast<mechanics::sea_t>(utils::flip_rb(sea_value)), color);
        }
        sea_file.close();
    }

    auto load_river_tiles(mechanics::data &d) -> void {
        std::ifstream river_tiles_file("assets/river_tiles.txt");
        if (!river_tiles_file.is_open()) {
            throw std::runtime_error("Failed to open river tiles file: " "river_tiles.txt");
        }
        while (!river_tiles_file.eof()) {
            std::string ignore;
            std::getline(river_tiles_file, ignore, ':');
            if (ignore == "\n") { break; }
            uint_fast32_t color{};
            river_tiles_file >> color;
            std::getline(river_tiles_file, ignore, ':');
            uint_fast32_t river_value{};
            river_tiles_file >> river_value;
            color = utils::flip_rb(color);
            auto &province = d.emplace_province(mechanics::koppen_t::none, mechanics::elevation_t::none,
                mechanics::vegetation_t::none, mechanics::soil_t::none, mechanics::sea_t::river, color);
            province.set_value(static_cast<uint_fast8_t>(river_value));
        }
        river_tiles_file.close();
    }

    auto load_river_borders(const mechanics::data &d) -> void {
        std::ifstream rivers_file("assets/rivers.txt");
        if (!rivers_file.is_open()) { throw std::runtime_error("Failed to open rivers file: " "rivers.txt"); }
        while (!rivers_file.eof()) {
            std::string ignore;
            std::getline(rivers_file, ignore, ':');
            if (ignore == "\n") { break; }
            uint_fast32_t color{};
            rivers_file >> color;
            std::getline(rivers_file, ignore, ':');
            uint_fast32_t neighbor_color{};
            rivers_file >> neighbor_color;
            std::getline(rivers_file, ignore, ':');
            uint_fast32_t river_value{};
            rivers_file >> river_value;
            color = utils::flip_rb(color);
            neighbor_color = utils::flip_rb(neighbor_color);
            d.province_at(color).add_river_neighbor(d.province_at(neighbor_color),
                static_cast<uint_fast8_t>(river_value));
        }
        rivers_file.close();
    }

    auto load_impassable_neighbors(const mechanics::data &d) -> void {
        std::ifstream impassable_file("assets/impassable_crossings.txt");
        if (!impassable_file.is_open()) {
            throw std::runtime_error("Failed to open impassable neighbors file: " "impassable_crossings.txt");
        }
        while (!impassable_file.eof()) {
            std::string ignore;
            std::getline(impassable_file, ignore, ':');
            if (ignore == "\n") { break; }
            uint_fast32_t color{};
            impassable_file >> color;
            std::getline(impassable_file, ignore, ':');
            uint_fast32_t neighbor_color{};
            impassable_file >> neighbor_color;
            color = utils::flip_rb(color);
            neighbor_color = utils::flip_rb(neighbor_color);
            d.province_at(color).add_impassable_neighbor(d.province_at(neighbor_color));
        }
        impassable_file.close();
    }

    auto process_pixel(
        std::unordered_map<std::reference_wrapper<mechanics::province>, std::vector<std::array<uint_fast32_t, 2UZ>>> &
        pixels_by_province,
        const processing::image &map_image, const mechanics::data &d, const uint_fast32_t color,
        const std::array<uint_fast32_t, 2UZ> &position,
        std::vector<uint8_t> &crossing_bytes) -> void {
        const auto i = static_cast<int_fast32_t>(position[0UZ]);
        const auto j = static_cast<int_fast32_t>(position[1UZ]);

        auto &this_province = d.province_at(color);
        this_province.expand_bounds(position);
        pixels_by_province[this_province].push_back(position);

        auto impassable_neighbor = false;

        std::ranges::for_each(std::views::iota(-1, 1 + 1), [&](const int_fast8_t dx) {
            std::ranges::for_each(std::views::iota(-1, 1 + 1), [&](const int_fast8_t dy) {
                if (dx == 0 && dy == 0) { return; }
                const auto ni = i + dx;
                const auto nj = j + dy;
                if (ni < 0 || std::cmp_greater_equal(ni, map_image.width()) ||
                    nj < 0 || std::cmp_greater_equal(nj, map_image.height())) { return; }
                const auto neighbor_color = map_image.color(static_cast<uint_fast32_t>(ni),
                    static_cast<uint_fast32_t>(nj));
                if (neighbor_color == color) { return; }
                auto &neighbor = d.province_at(neighbor_color);
                this_province.add_neighbor(neighbor);
                if (!impassable_neighbor && this_province.
                    impassable_neighbors()
                    .contains(neighbor)) { impassable_neighbor = true; }
            });
        });
        if (impassable_neighbor) {
            crossing_bytes[4UZ * (position[0UZ] + position[1UZ] * map_image.width())] = 255U;
            crossing_bytes[4UZ * (position[0UZ] + position[1UZ] * map_image.width()) + 1UZ] = 0U;
            crossing_bytes[4UZ * (position[0UZ] + position[1UZ] * map_image.width()) + 2UZ] = 0U;
            crossing_bytes[4UZ * (position[0UZ] + position[1UZ] * map_image.width()) + 3UZ] = 255U;
        }
        else {
            crossing_bytes[4UZ * (position[0UZ] + position[1UZ] * map_image.width())] = 0U;
            crossing_bytes[4UZ * (position[0UZ] + position[1UZ] * map_image.width()) + 1UZ] = 0U;
            crossing_bytes[4UZ * (position[0UZ] + position[1UZ] * map_image.width()) + 2UZ] = 0U;
            crossing_bytes[4UZ * (position[0UZ] + position[1UZ] * map_image.width()) + 3UZ] = 0U;
        }
    }
}

namespace processing {
    auto load_image(mechanics::data &d, image &map_image, std::vector<uint8_t> &crossing_bytes,
        std::string &loading_text) -> void {
        loading_text = "Loading provinces file...";
        load_provinces(d);

        loading_text = "Loading sea tiles file...";
        load_seas(d);

        loading_text = "Loading river tiles file...";
        load_river_tiles(d);

        d.lock_provinces();

        loading_text = "Loading rivers file...";
        load_river_borders(d);

        loading_text = "Loading impassable neighbors file...";
        load_impassable_neighbors(d);

        map_image = image{"assets/provinces_generated.png"};

        std::unordered_map<std::reference_wrapper<mechanics::province>, std::vector<std::array<uint_fast32_t, 2UZ>>>
            pixels_by_province;

        loading_text = "Processing pixels...";
        crossing_bytes = std::vector<uint8_t>(4UZ * map_image.width() * map_image.height());
        std::ranges::for_each(std::views::iota(0U, map_image.width()), [&](const uint_fast32_t i) {
            std::ranges::for_each(std::views::iota(0U, map_image.height()), [&](const uint_fast32_t j) {
                process_pixel(pixels_by_province, map_image, d, map_image.color(i, j), {i, j}, crossing_bytes);
            });
        });

        auto &province_values = d.provinces();

        loading_text = "Finalizing provinces...";
        std::for_each(std::execution::unseq, province_values.begin(), province_values.end(),
            [&](mechanics::province &province) {
                if (province.size() == 0U) { return; }
                province.finalize(pixels_by_province.at(province));
            });
        std::for_each(std::execution::par_unseq, province_values.begin(), province_values.end(),
            [](mechanics::province &province) { province.process_distances(); });

        loading_text.clear();
    }
}
