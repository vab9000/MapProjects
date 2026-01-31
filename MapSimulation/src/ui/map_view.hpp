#ifndef UI_MAP_VIEW
#define UI_MAP_VIEW
#include <SFML/Graphics.hpp>

namespace mechanics {
    class province;
}

namespace ui {
    enum class map_mode_t : unsigned char {
        provinces, owner, koppen,
        elevation, roughness, vegetation,
        soil, sea, river_size
    };

    enum class pixel_flag_t : unsigned int {
        impassable = 0x01000000U, bridge = 0x02000000U, water = 0x00000100U,
        outline = 0x00000001U,
    };

    class map_view {
    public:
        inline static auto map_mode{map_mode_t::provinces};
        inline static sf::Texture province_colors{};
        inline static sf::Texture pixel_flags{};

        static auto select_province(mechanics::province &p) -> void;

        static auto unselect_province() -> void;

        static auto selected_province() -> mechanics::province *;

        static auto set_province_colors_size() -> void;

        static auto update_province_color(const mechanics::province &p) -> void;

        static auto update_all_province_colors() -> void;

        static auto set_pixel_flags_size(unsigned int width, unsigned int height) -> void;

        static auto set_pixel_flags(std::pair<const unsigned int, const unsigned int> pixel, pixel_flag_t flag,
            bool clear) -> void;

        static auto update_pixel_flags_texture() -> void;
    };
}
#endif
