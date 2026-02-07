#include "saver.hpp"
#include <functional>
#include "../graphics/image.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <array>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>
#include <sstream>
#include <stb_image_write.h>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "control.hpp"
#include "location.hpp"
#include "zstring_view.hpp"
#include "../graphics/paletted_texture.hpp"

namespace logic {
    static auto save_properties(std::ofstream &file) {
        const auto properties = location::properties();
        std::println(file, "PROPERTIES");
        for (const auto &[values, name] : properties) {
            std::println(file, "{}", name);
            std::println(file, "{}", values.size());
            for (const auto &[color, value_name] : values) {
                std::println(file, "{}", value_name);
                std::println(file, "{} {} {} {}", static_cast<int>(color.at(0)),
                    static_cast<int>(color.at(1)),
                    static_cast<int>(color.at(2)),
                    static_cast<int>(color.at(3)));
            }
        }
    }

    static auto save_locations(std::ofstream &file) {
        std::println(file, "LOCATIONS");
        for (const auto &loc : location::locations()) {
            std::println(file, "LOCATION");
            std::println(file, "{}", loc.name());
            const auto color = loc.base_color();
            std::println(file, "{} {} {} {}", static_cast<int>(color.at(0)),
                static_cast<int>(color.at(1)),
                static_cast<int>(color.at(2)),
                static_cast<int>(color.at(3)));
            const auto properties = location::properties();
            for (size_t i = 0; i < properties.size(); ++i) {
                if (loc.prop_value(i) == 0) { continue; }
                std::println(file, "{}: {}", i, loc.prop_value(i));
            }
        }
    }

    static auto save_image(const zstring_view filepath) {
        const auto &canvas = control::canvas();
        const auto dims = canvas.dims();
        auto data = std::vector<unsigned char>(4UZ * dims.at(0) * dims.at(1));
        for (auto row = 0; row < dims.at(1); ++row) {
            for (auto col = 0; col < dims.at(0); ++col) {
                const auto color_idx = canvas.index({col, row});
                const auto color = location::locations()[color_idx].base_color();
                const auto offset = 4UZ * (row * dims.at(0) + col);
                data.at(offset + 0) = color.at(0);
                data.at(offset + 1) = color.at(1);
                data.at(offset + 2) = color.at(2);
                data.at(offset + 3) = color.at(3);
            }
        }
        stbi_write_bmp(filepath.str(), dims.at(0), dims.at(1), 4, data.data());
    }

    auto saver::save(const zstring_view folder) -> void {
        const std::filesystem::path folder_path{folder.string()};
        create_directories(folder_path);
        std::ofstream properties_file{folder_path / "properties.txt"};
        if (!properties_file.is_open()) { throw std::runtime_error("Failed to open properties file for saving."); }
        save_properties(properties_file);
        std::ofstream locations_file{folder_path / "locations.txt"};
        if (!locations_file.is_open()) { throw std::runtime_error("Failed to open locations file for saving."); }
        save_locations(locations_file);
        save_image(zstring_view(folder.string() + "/map.bmp"));
    }

    auto saver::load(const zstring_view folder) -> void {
        location::properties_.clear();
        location::locations_.clear();
        location::free_locations_.clear();
        location::current_property_ = location::base_color_index;
        control::selected_idx_ = location::base_color_index;
        control::property_value_idx_ = 0;
        const std::filesystem::path folder_path{folder.string()};
        std::ifstream properties_file{folder_path / "properties.txt"};
        if (!properties_file.is_open()) { throw std::runtime_error("Failed to open properties file for loading."); }
        load_properties(properties_file);
        std::ifstream locations_file{folder_path / "locations.txt"};
        if (!locations_file.is_open()) { throw std::runtime_error("Failed to open locations file for loading."); }
        load_locations(locations_file);
        load_image(zstring_view(folder.string() + "/map.bmp"));
    }

    auto saver::load_properties(std::istream &file) -> void {
        std::string header;
        std::getline(file, header);
        if (header != "PROPERTIES") { throw std::runtime_error("Invalid properties file format."); }
        while (!file.eof()) {
            std::string name;
            std::getline(file, name);
            if (name.empty()) { continue; }
            location::properties_.emplace_back(std::vector<location::value>{}, std::move(name));
            std::string count_str;
            std::getline(file, count_str);
            const auto count = std::stoull(count_str);
            for (auto i = 0UZ; i < count; i++) {
                std::string value_name;
                std::getline(file, value_name);
                std::string color_line;
                std::getline(file, color_line);
                std::stringstream color_stream{color_line};
                std::array<unsigned int, 4> color = {};
                color_stream >> color.at(0) >> color.at(1) >> color.at(2) >> color.at(3);
                location::properties_.back().values.emplace_back(
                    std::array{
                        static_cast<unsigned char>(color.at(0)),
                        static_cast<unsigned char>(color.at(1)),
                        static_cast<unsigned char>(color.at(2)),
                        static_cast<unsigned char>(color.at(3))
                    }, std::move(value_name));
            }
        }
    }

    auto saver::load_locations(std::istream &file) -> void {
        std::string header;
        std::getline(file, header);
        if (header != "LOCATIONS") { throw std::runtime_error("Invalid locations file format."); }
        std::getline(file, header); // Clear first location header.
        auto &canvas = control::canvas();
        while (!file.eof()) {
            std::string name;
            std::getline(file, name);
            location::locations_.emplace_back(location::locations_.size());
            location::locations_.back().property_values_.resize(location::properties_.size(), 0);
            std::string color_line;
            std::getline(file, color_line);
            std::stringstream color_stream{color_line};
            std::array<unsigned int, 4> color = {};
            color_stream >> color.at(0) >> color.at(1) >> color.at(2) >> color.at(3);
            location::locations_.back().base_color_ = {
                static_cast<unsigned char>(color.at(0)), static_cast<unsigned char>(color.at(1)),
                static_cast<unsigned char>(color.at(2)), static_cast<unsigned char>(color.at(3))
            };
            canvas.set_color(location::locations_.size() - 1, location::locations_.back().base_color_);
            std::string line;
            while (std::getline(file, line) && !line.empty() && line != "LOCATION") {
                const auto delimiter_pos = line.find(':');
                if (delimiter_pos == std::string::npos) { continue; }
                const auto prop_index = std::stoull(line.substr(0, delimiter_pos));
                const auto value_index = std::stoull(line.substr(delimiter_pos + 1));
                location::locations_.back().property_values_.at(prop_index) = value_index;
            }
        }
    }

    struct color_hash {
        auto operator()(const std::array<unsigned char, 4> &color) const -> std::size_t {
            return std::hash<unsigned int>::operator()(static_cast<unsigned int>(color.at(0)) << 24) |
                   static_cast<unsigned int>(color.at(1)) << 16 |
                   static_cast<unsigned int>(color.at(2)) << 8 |
                   static_cast<unsigned int>(color.at(3));
        }
    };

    auto saver::load_image(const zstring_view filepath) -> void {
        auto &canvas = control::canvas();
        const auto dims = canvas.dims();
        graphics::image img{filepath};
        std::unordered_map<std::array<unsigned char, 4>, size_t, color_hash> color_to_location;
        for (size_t loc_idx = 0; loc_idx < location::locations_.size(); ++loc_idx) {
            const auto loc_color = location::locations_.at(loc_idx).base_color();
            color_to_location[loc_color] = loc_idx;
        }
        std::vector<unsigned int> data(static_cast<size_t>(dims.at(0) * dims.at(1)));
        for (auto row = 0; row < dims.at(1); ++row) {
            for (auto col = 0; col < dims.at(0); ++col) {
                const auto color = img.at({col, row});
                const auto color_array = std::array{
                    color[0], color[1], color[2], color[3]
                };
                const auto loc_index = color_to_location.at(color_array);
                location::locations_.at(loc_index).size_ += 1;
                data.at(col + row * dims.at(0)) = loc_index;
            }
        }
        for (const location &loc : location::locations_) {
            if (loc.size_ == 0) {
                location::free_locations_.emplace_back(loc.idx_);
                std::println("Warning: Location '{}' has size 0 and has been marked as free.", loc.idx());
            }
        }
        canvas.edit(data, {0, 0}, dims.at(0));
    }
} // namespace logic
