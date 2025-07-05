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
    sf::Sprite map_sprite_one_{texture_};
    sf::Sprite map_sprite_two_{texture_};

public:
    explicit drawing(data &data, simulation &simulation, const std::string &loading_text);

    bool init_sprites(const image &map_image, const std::vector<uint8_t> &bytes);

    void recalculate_sprite_coords(std::array<int_fast32_t, 2> offset, double_t zoom, uint_fast32_t map_width);

    void draw_loading_message(sf::RenderWindow &window) const;

    void draw_map(sf::RenderWindow &window) const;

    void update_map_texture(const uint8_t *bytes);

    void update_map_texture(const uint8_t *bytes, const sf::Vector2u &dimensions,
                            const sf::Vector2u &position);

    void draw_gui(sf::RenderWindow &window) const;
};
