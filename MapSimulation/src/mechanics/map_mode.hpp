#pragma once
#include <atomic>
#include <cstdint>
#include <SFML/Graphics.hpp>

namespace mechanics {
    class province;

    enum class map_mode_t : uint_fast8_t {
        provinces,
        owner,
        koppen,
        elevation,
        vegetation,
        soil,
        sea,
    };

    extern std::atomic<map_mode_t> map_mode;

    extern sf::Texture province_colors;

    auto set_province_colors_size() -> void;

    auto update_province_color(const province &p) -> void;

    auto update_all_province_colors() -> void;
}
