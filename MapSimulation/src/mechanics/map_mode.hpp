#ifndef MECHANICS_MAP_MODE
#define MECHANICS_MAP_MODE
#include <SFML/Graphics.hpp>

namespace mechanics {
    class province;

    enum class map_mode_t : unsigned char {
        provinces, owner, koppen,
        elevation, roughness, vegetation,
        soil, sea, river_size,
    };

    enum class pixel_flag_t : unsigned int {
        impassable = 0x01000000U, bridge = 0x02000000U, water = 0x00000100U,
        outline = 0x00000001U,
    };

    extern map_mode_t map_mode;

    extern sf::Texture province_colors;

    extern sf::Texture pixel_flags;

    auto set_province_colors_size() -> void;

    auto update_province_color(const province &p) -> void;

    auto update_all_province_colors() -> void;

    auto set_pixel_flags_size(unsigned int width, unsigned int height) -> void;

    auto set_pixel_flags(std::pair<const unsigned int, const unsigned int> pixel, pixel_flag_t flag,
        bool clear) -> void;

    auto update_pixel_flags_texture() -> void;
}
#endif
