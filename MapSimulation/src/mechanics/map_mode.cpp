#include "map_mode.hpp"
#include "data.hpp"
#include "province.hpp"

namespace mechanics {
    std::atomic<map_mode_t> map_mode{map_mode_t::provinces};

    sf::Texture province_colors;

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
}
