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
            uint_fast32_t color;
            province_file >> color;
            std::getline(province_file, ignore, ':');
            uint_fast32_t koppen_value;
            province_file >> koppen_value;
            std::getline(province_file, ignore, ':');
            uint_fast32_t elevation_value;
            province_file >> elevation_value;
            std::getline(province_file, ignore, ':');
            uint_fast32_t vegetation_value;
            province_file >> vegetation_value;
            std::getline(province_file, ignore, ':');
            uint_fast32_t soil_value;
            province_file >> soil_value;
            std::getline(province_file, ignore);
            color = utils::flip_rb(color);
            d.provinces().emplace(std::piecewise_construct, std::forward_as_tuple(color), std::forward_as_tuple(
                                      color, static_cast<mechanics::koppen_t>(utils::flip_rb(koppen_value)),
                                      static_cast<mechanics::elevation_t>(utils::flip_rb(elevation_value)),
                                      static_cast<mechanics::vegetation_t>(utils::flip_rb(vegetation_value)),
                                      static_cast<mechanics::soil_t>(utils::flip_rb(soil_value)),
                                      mechanics::sea_t::none));
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
            uint_fast32_t color;
            sea_file >> color;
            std::getline(sea_file, ignore, ':');
            uint_fast32_t sea_value;
            sea_file >> sea_value;
            color = utils::flip_rb(color);
            d.provinces().emplace(std::piecewise_construct, std::forward_as_tuple(color), std::forward_as_tuple(
                                      color, mechanics::koppen_t::none, mechanics::elevation_t::none,
                                      mechanics::vegetation_t::none,
                                      mechanics::soil_t::none,
                                      static_cast<mechanics::sea_t>(utils::flip_rb(sea_value))));
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
            uint_fast32_t color;
            river_tiles_file >> color;
            std::getline(river_tiles_file, ignore, ':');
            uint_fast32_t river_value;
            river_tiles_file >> river_value;
            color = utils::flip_rb(color);
            d.provinces().emplace(std::piecewise_construct, std::forward_as_tuple(color), std::forward_as_tuple(
                                      color, mechanics::koppen_t::none, mechanics::elevation_t::none,
                                      mechanics::vegetation_t::none,
                                      mechanics::soil_t::none, mechanics::sea_t::river));
            d.provinces().at(color).set_value(river_value);
        }
        river_tiles_file.close();
    }

    auto load_river_borders(mechanics::data &d) -> void {
        std::ifstream rivers_file("assets/rivers.txt");
        if (!rivers_file.is_open()) { throw std::runtime_error("Failed to open rivers file: " "rivers.txt"); }
        while (!rivers_file.eof()) {
            std::string ignore;
            std::getline(rivers_file, ignore, ':');
            if (ignore == "\n") { break; }
            uint_fast32_t color;
            rivers_file >> color;
            std::getline(rivers_file, ignore, ':');
            uint_fast32_t neighbor_color;
            rivers_file >> neighbor_color;
            std::getline(rivers_file, ignore, ':');
            uint_fast32_t river_value;
            rivers_file >> river_value;
            color = utils::flip_rb(color);
            neighbor_color = utils::flip_rb(neighbor_color);
            d.provinces().at(color).add_river_neighbor(d.provinces().at(neighbor_color),
                                                       static_cast<uint_fast8_t>(river_value));
        }
        rivers_file.close();
    }

    auto load_impassable_neighbors(mechanics::data &d) -> void {
        std::ifstream impassable_file("assets/impassable_crossings.txt");
        if (!impassable_file.is_open()) {
            throw std::runtime_error("Failed to open impassable neighbors file: " "impassable_crossings.txt");
        }
        while (!impassable_file.eof()) {
            std::string ignore;
            std::getline(impassable_file, ignore, ':');
            if (ignore == "\n") { break; }
            uint_fast32_t color;
            impassable_file >> color;
            std::getline(impassable_file, ignore, ':');
            uint_fast32_t neighbor_color;
            impassable_file >> neighbor_color;
            color = utils::flip_rb(color);
            neighbor_color = utils::flip_rb(neighbor_color);
            d.provinces().at(color).add_impassable_neighbor(d.provinces().at(neighbor_color));
        }
        impassable_file.close();
    }

    auto process_pixel(
        std::unordered_map<std::reference_wrapper<mechanics::province>, std::vector<std::array<uint_fast32_t, 2> > > &
        pixels_by_province,
        const processing::image &map_image, mechanics::data &d, const uint_fast32_t color,
        const std::array<uint_fast32_t, 2> &position,
        std::vector<uint8_t> &crossing_bytes) -> void {
        const auto i = static_cast<int_fast32_t>(position[0]);
        const auto j = static_cast<int_fast32_t>(position[1]);

        auto &this_province = d.provinces().at(color);
        this_province.expand_bounds(position);
        pixels_by_province[this_province].push_back(position);

        auto impassable_neighbor = false;

        std::ranges::for_each(std::views::iota(-1, 1 + 1), [&](const int_fast8_t dx) {
            std::ranges::for_each(std::views::iota(-1, 1 + 1), [&](const int_fast8_t dy) {
                if (dx == 0 && dy == 0) { return; }
                const auto ni = i + dx, nj = j + dy;
                if (ni < 0 || ni >= map_image.width() ||
                    nj < 0 || nj >= map_image.height()) { return; }
                const auto neighbor_color = map_image.color(ni, nj);
                if (neighbor_color == color) { return; }
                auto &neighbor = d.provinces().at(neighbor_color);
                this_province.add_neighbor(neighbor);
                if (!impassable_neighbor && this_province.
                    impassable_neighbors()
                    .contains(neighbor)) { impassable_neighbor = true; }
            });
        });
        if (impassable_neighbor) {
            crossing_bytes[4ULL * (i + j * map_image.width())] = 255;
            crossing_bytes[4ULL * (i + j * map_image.width()) + 1] = 0;
            crossing_bytes[4ULL * (i + j * map_image.width()) + 2] = 0;
            crossing_bytes[4ULL * (i + j * map_image.width()) + 3] = 255;
        } else {
            crossing_bytes[4ULL * (i + j * map_image.width())] = 0;
            crossing_bytes[4ULL * (i + j * map_image.width()) + 1] = 0;
            crossing_bytes[4ULL * (i + j * map_image.width()) + 2] = 0;
            crossing_bytes[4ULL * (i + j * map_image.width()) + 3] = 0;
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

        loading_text = "Loading rivers file...";
        load_river_borders(d);

        loading_text = "Loading river tiles file...";
        load_river_tiles(d);

        loading_text = "Loading impassable neighbors file...";
        load_impassable_neighbors(d);

        std::unordered_map<std::reference_wrapper<mechanics::province>, std::vector<std::array<uint_fast32_t, 2> > >
                pixels_by_province;

        map_image = image{"assets/provinces_generated.png"};

        loading_text = "Processing pixels...";
        crossing_bytes = std::vector<uint8_t>(4ULL * map_image.width() * map_image.height());
        std::ranges::for_each(std::views::iota(0U, map_image.width()), [&](const uint_fast32_t i) {
            std::ranges::for_each(std::views::iota(0U, map_image.height()), [&](const uint_fast32_t j) {
                process_pixel(pixels_by_province, map_image, d, map_image.color(i, j), {i, j}, crossing_bytes);
            });
        });

        auto province_values = d.provinces() | std::views::values;

        loading_text = "Finalizing provinces...";
        std::for_each(std::execution::par_unseq, province_values.begin(), province_values.end(),
                      [&pixels_by_province](mechanics::province &province) {
                          province.finalize(pixels_by_province.at(province));
                      });
        std::for_each(std::execution::par_unseq, province_values.begin(), province_values.end(),
                      [](mechanics::province &province) { province.process_distances(); });

        loading_text.clear();
    }
}
