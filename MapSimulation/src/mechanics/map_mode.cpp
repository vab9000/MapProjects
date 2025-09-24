#include "map_mode.hpp"
#include "data.hpp"
#include "province.hpp"

namespace mechanics {
    std::atomic<map_mode_t> map_mode{map_mode_t::provinces};

    sf::Texture province_colors;

    sf::Texture pixel_flags;

    static sf::Vector2u dimensions;

    auto set_province_colors_size() -> void {
        const auto num_provinces = data::instance().provinces().size();
        if (const auto dim = static_cast<unsigned int>(sqrt(num_provinces)) + 1U; !province_colors.resize({dim, dim})) {
            throw std::runtime_error("Failed to allocate memory for provinces.");
        }
        dimensions = province_colors.getSize();
    }

    auto update_province_color(const province &p) -> void {
        const auto color = p.color();
        const std::array<unsigned char, 4U> color_array{
            static_cast<unsigned char>(color >> 16), static_cast<unsigned char>(color >> 8),
            static_cast<unsigned char>(color), 255U
        };
        const auto index = p.id() * 4U;
        province_colors.update(color_array.data(), {1U, 1U}, {
            static_cast<unsigned int>(index % dimensions.x), static_cast<unsigned int>(index / dimensions.x)
        });
    }

    auto update_all_province_colors() -> void {
        const auto &provinces = data::instance().provinces();
        std::vector<unsigned char> colors(4UZ * dimensions.x * dimensions.y);
        for (const auto &p : provinces) {
            const auto color = p.color();
            const auto index = p.id() * 4UZ;
            colors[index + 3UZ] = 255U;
            colors[index + 2UZ] = static_cast<unsigned char>(color);
            colors[index + 1UZ] = static_cast<unsigned char>(color >> 8);
            colors[index] = static_cast<unsigned char>(color >> 16);
        }
        province_colors.update(colors.data(), {dimensions.x, dimensions.y}, {
            0U, 0U
        });
    }

    static sf::Vector2u pixel_flags_dimensions;

    static std::vector<unsigned char> pixel_flags_data;

    auto set_pixel_flags_size(const unsigned int width, const unsigned int height) -> void {
        pixel_flags_dimensions = {width, height};
        pixel_flags_data.resize(4UZ * width * height, 0U);
        if (!pixel_flags.resize({width, height})) {
            throw std::runtime_error("Failed to allocate memory for pixel flags.");
        }
    }

    auto set_pixel_flags(const std::pair<const unsigned int, const unsigned int> pixel, const pixel_flag_t flag,
        const bool clear) -> void {
        const auto index = 4UZ * (pixel.first + pixel.second * pixel_flags_dimensions.x);
        const auto flag_value = static_cast<unsigned int>(flag);
        if (clear) {
            pixel_flags_data[index] &= ~flag_value >> 24U;
            pixel_flags_data[index + 1UZ] &= ~flag_value >> 16U;
            pixel_flags_data[index + 2UZ] &= ~flag_value >> 8U;
            pixel_flags_data[index + 3UZ] &= ~flag_value;
        }
        else {
            pixel_flags_data[index] |= flag_value >> 24U;
            pixel_flags_data[index + 1UZ] |= flag_value >> 16U;
            pixel_flags_data[index + 2UZ] |= flag_value >> 8U;
            pixel_flags_data[index + 3UZ] |= flag_value;
        }
    }

    auto update_pixel_flags_texture() -> void {
        pixel_flags.update(pixel_flags_data.data(), {pixel_flags_dimensions.x, pixel_flags_dimensions.y}, {0U, 0U});
    }
}
