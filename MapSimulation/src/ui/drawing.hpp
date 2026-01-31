#ifndef UI_DRAWING_HPP
#define UI_DRAWING_HPP
#include <SFML/Graphics.hpp>
#include "map_view.hpp"
#include "../processing/simulation.hpp"

namespace ui {
    class drawing {
        inline static const std::string &loading_text = processing::simulation::loading_text();

        inline static sf::Texture map_texture_{};
        inline static sf::Sprite map_sprite_{map_texture_};
        inline static sf::Shader map_shader_{};
        inline static sf::Sprite flags_sprite_{map_view::pixel_flags};
        inline static sf::Shader flags_shader_{};

        inline static auto draw_crossings_ = true;
        inline static auto draw_outline_ = true;
        inline static auto draw_bridges_ = false;

        static auto draw_checkboxes() -> void;

    public:
        // Create a texture from the map image and initialize sprites
        static auto init_sprites(const processing::image &map_image, const std::vector<unsigned char> &bytes) -> bool;

        // Recalculate the coordinates of the sprites based on the offset and zoom level
        static auto recalculate_sprite_coords(std::array<int, 2UZ> offset, double zoom) -> void;

        // Draw the loading message on the window
        static auto draw_loading_message(sf::RenderWindow &window) -> void;

        // Draw the map using the sprites
        static auto draw_map(sf::RenderWindow &window) -> void;

        // Draw the GUI elements on the window
        static auto draw_gui(const sf::RenderWindow &window) -> void;
    };
}
#endif
