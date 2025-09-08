#include "load_image.hpp"
#include <algorithm>
#include <execution>
#include <fstream>
#include <iostream>
#include <ranges>
#include "image.hpp"
#include "../mechanics/data.hpp"

namespace {
    auto load_provinces(mechanics::data &d) -> void {
        std::ifstream province_file("assets/provinces.txt");
        if (!province_file.is_open()) { throw std::runtime_error("Failed to open province file: " "provinces.txt"); }
        while (!province_file.eof()) {
            std::string ignore;
            std::getline(province_file, ignore, ':');
            if (ignore == "\n") { break; }
            unsigned int color{};
            province_file >> color;
            std::getline(province_file, ignore, ':');
            unsigned int koppen_value{};
            province_file >> koppen_value;
            std::getline(province_file, ignore, ':');
            unsigned int elevation_value{};
            province_file >> elevation_value;
            std::getline(province_file, ignore, ':');
            unsigned int vegetation_value{};
            province_file >> vegetation_value;
            std::getline(province_file, ignore, ':');
            unsigned int soil_value{};
            province_file >> soil_value;
            std::getline(province_file, ignore);
            d.emplace_province(static_cast<mechanics::koppen_t>(koppen_value),
                static_cast<mechanics::elevation_t>(elevation_value),
                static_cast<mechanics::vegetation_t>(vegetation_value),
                static_cast<mechanics::soil_t>(soil_value),
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
            unsigned int color{};
            sea_file >> color;
            std::getline(sea_file, ignore, ':');
            unsigned int sea_value{};
            sea_file >> sea_value;
            d.emplace_province(mechanics::koppen_t::none, mechanics::elevation_t::none,
                mechanics::vegetation_t::none, mechanics::soil_t::none,
                static_cast<mechanics::sea_t>(sea_value), color);
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
            unsigned int color{};
            river_tiles_file >> color;
            std::getline(river_tiles_file, ignore, ':');
            unsigned int river_value{};
            river_tiles_file >> river_value;
            auto &province = d.emplace_province(mechanics::koppen_t::none, mechanics::elevation_t::none,
                mechanics::vegetation_t::none, mechanics::soil_t::none, mechanics::sea_t::river, color);
            province.set_value(static_cast<unsigned char>(river_value));
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
            unsigned int color{};
            rivers_file >> color;
            std::getline(rivers_file, ignore, ':');
            unsigned int neighbor_color{};
            rivers_file >> neighbor_color;
            std::getline(rivers_file, ignore, ':');
            unsigned int river_value{};
            rivers_file >> river_value;
            d.province_at(color).add_river_neighbor(d.province_at(neighbor_color),
                static_cast<unsigned char>(river_value));
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
            unsigned int color{};
            impassable_file >> color;
            std::getline(impassable_file, ignore, ':');
            unsigned int neighbor_color{};
            impassable_file >> neighbor_color;
            d.province_at(color).add_impassable_neighbor(d.province_at(neighbor_color));
        }
        impassable_file.close();
    }

    auto process_pixel(
        std::unordered_map<utils::ref<mechanics::province>, std::vector<std::array<unsigned int, 2UZ>>, utils::ref_hash<
            mechanics::province>> &
        pixels_by_province,
        const processing::image &map_image, const mechanics::data &d, const unsigned int color,
        const std::array<unsigned int, 2UZ> &position,
        std::vector<unsigned char> &crossing_bytes) -> void {
        auto &this_province = d.province_at(color);
        this_province.expand_bounds(position);
        pixels_by_province[this_province].push_back(position);

        auto impassable_neighbor = false;

        std::ranges::for_each(std::views::iota(0U, 2U + 1U), [&](const unsigned int dx) {
            std::ranges::for_each(std::views::iota(0U, 2U + 1U), [&](const unsigned int dy) {
                if (dx == 1U && dy == 1U) { return; }
                if ((position[0UZ] == 0U && dx == 0U) || (position[1UZ] == 0U && dy == 0U)) { return; }
                const auto ni = position[0UZ] + dx - 1U;
                const auto nj = position[1UZ] + dy - 1U;
                if (ni >= map_image.width() ||
                    nj >= map_image.height()) { return; }
                const auto neighbor_color = map_image.color(static_cast<unsigned int>(ni),
                    static_cast<unsigned int>(nj));
                if (neighbor_color == color) { return; }
                auto &neighbor = d.province_at(neighbor_color);
                this_province.add_neighbor(neighbor);
                if (!impassable_neighbor && this_province.impassable_neighbors()
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
    auto load_image(mechanics::data &d, image &map_image, std::vector<unsigned char> &crossing_bytes,
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

        std::unordered_map<utils::ref<mechanics::province>, std::vector<std::array<unsigned int, 2UZ>>, utils::ref_hash<
                mechanics::province>>
            pixels_by_province;

        loading_text = "Processing pixels...";
        crossing_bytes = std::vector<unsigned char>(4UZ * map_image.width() * map_image.height());
        std::ranges::for_each(std::views::iota(0U, map_image.width()), [&](const unsigned int i) {
            std::ranges::for_each(std::views::iota(0U, map_image.height()), [&](const unsigned int j) {
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
