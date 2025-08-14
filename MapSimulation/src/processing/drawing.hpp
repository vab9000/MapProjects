#pragma once

#include <SFML/Graphics.hpp>
#include "../mechanics/map_mode.hpp"

namespace mechanics {
    class data;
}

namespace processing {
    class image;
    class simulation;

    class drawing {
        simulation &simulation_;
        const std::string &loading_text_;

        sf::Texture texture_;
        sf::Sprite map_sprite_{texture_};
        sf::Shader map_shader_;
        sf::Texture crossing_texture_;
        sf::Sprite crossing_sprite_{crossing_texture_};
        sf::Shader crossing_shader_;

        bool draw_crossings_{true};
        bool draw_outline_{true};

        auto draw_checkboxes() -> void;

    public:
        explicit drawing(simulation &simulation, const std::string &loading_text);

        // Create a texture from the map image and initialize sprites
        auto init_sprites(const image &map_image, const std::vector<uint8_t> &bytes,
            const std::vector<uint8_t> &crossing_bytes) -> bool;

        // Recalculate the coordinates of the sprites based on the offset and zoom level
        auto recalculate_sprite_coords(std::array<int, 2UZ> offset, double zoom) -> void;

        // Draw the loading message on the window
        auto draw_loading_message(sf::RenderWindow &window) const -> void;

        // Draw the map using the sprites
        auto draw_map(sf::RenderWindow &window) -> void;

        // Draw the GUI elements on the window
        auto draw_gui(const sf::RenderWindow &window) -> void;
    };
}
