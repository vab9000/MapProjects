#include "map_mode.hpp"
#include "data.hpp"
#include "province.hpp"

namespace mechanics {
    std::atomic<map_mode_t> map_mode{map_mode_t::provinces};

    sf::Texture province_colors;

    static sf::Vector2u dimensions;

    auto set_province_colors_size() -> void {
        const auto num_provinces = data::instance().provinces().size();
        if (const auto dim = static_cast<uint_fast32_t>(sqrt(num_provinces)) + 1U; !province_colors.resize({dim, dim})) {
            throw std::runtime_error("Failed to allocate memory for provinces.");
        }
        dimensions = province_colors.getSize();
    }

    auto update_province_color(const province &p) -> void {
        const auto color = p.color();
        const auto index = p.id() * 4U;
        province_colors.update(reinterpret_cast<const uint8_t *>(&color), {1U, 1U}, {
            static_cast<unsigned int>(index % dimensions.x), static_cast<unsigned int>(index / dimensions.x)
        });
    }

    auto update_all_province_colors() -> void {
        const auto &provinces = data::instance().provinces();
        std::vector<uint8_t> colors(4UZ * dimensions.x * dimensions.y);
        for (const auto &p : provinces) {
            const auto color = p.color();
            const auto index = p.id() * 4UZ;
            colors[index + 3UZ] = 255U;
            colors[index + 2UZ] = static_cast<uint8_t>(color >> 16);
            colors[index + 1UZ] = static_cast<uint8_t>(color >> 8);
            colors[index] = static_cast<uint8_t>(color);
        }
        province_colors.update(colors.data(), {dimensions.x, dimensions.y}, {
            0U, 0U
        });
    }
}
