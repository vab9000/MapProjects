#pragma once

#include <SFML/Graphics.hpp>

struct data;
class image;
class simulation;

class drawing {
    data &data_;
    simulation &simulation_;
    const std::string &loading_text_;

    sf::Texture texture_{};
    sf::Sprite map_sprite_{texture_};

public:
    explicit drawing(data &data, simulation &simulation, const std::string &loading_text);

    // Create a texture from the map image and initialize sprites
    bool init_sprites(const image &map_image, const std::vector<uint8_t> &bytes);

    // Recalculate the coordinates of the sprites based on the offset and zoom level
    void recalculate_sprite_coords(std::array<int_fast32_t, 2> offset, double_t zoom, uint_fast32_t map_width);

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
    void draw_gui(sf::RenderWindow &window) const;
};
