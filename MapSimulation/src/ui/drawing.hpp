#pragma once

#include <SFML/Graphics.hpp>

class data;
class image;
class simulation;

class drawing {
    simulation &simulation_;
    const std::string &loading_text_;

    sf::Texture base_map_;
    sf::Texture texture_;
    sf::Sprite map_sprite_{texture_};
    sf::Shader map_shader_;
    sf::Texture crossing_texture_;
    sf::Sprite crossing_sprite_{crossing_texture_};
    sf::Shader crossing_shader_;

    bool draw_crossings_ = true;

    void draw_checkboxes();

public:
    explicit drawing(simulation &simulation, const std::string &loading_text);

    // Create a texture from the map image and initialize sprites
    bool init_sprites(const image &map_image, const std::vector<uint8_t> &bytes,
                      const std::vector<uint8_t> &crossing_bytes);

    // Recalculate the coordinates of the sprites based on the offset and zoom level
    void recalculate_sprite_coords(std::array<int_fast32_t, 2> offset, double_t zoom);

    // Draw the loading message on the window
    void draw_loading_message(sf::RenderWindow &window) const;

    // Draw the map using the sprites
    void draw_map(sf::RenderWindow &window);

    // Update the map texture with new pixel data
    void update_map_texture(const uint8_t *bytes);

    // Update the map texture with new pixel data and position
    void update_map_texture(const uint8_t *bytes, const sf::Vector2u &dimensions,
                            const sf::Vector2u &position);

    // Draw the GUI elements on the window
    void draw_gui(sf::RenderWindow &window);
};
