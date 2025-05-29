#pragma once

#include <SFML/Graphics.hpp>

struct data;
class image;

class drawing {
    const data &data_;
    double &progress_;

    sf::Texture texture_{};
    sf::Sprite map_sprite_one_{texture_};
    sf::Sprite map_sprite_two_{texture_};

public:
    explicit drawing(data &data, double &progress);

    bool init_sprites(const image &map_image, const std::vector<unsigned char> &bytes);

    void recalculate_sprite_coords(std::array<int, 2> offset, double zoom, unsigned int map_width);

    void draw_loading_screen(sf::RenderWindow &window) const;

    void draw_map(sf::RenderWindow &window) const;

    void update_map_texture(const unsigned char *bytes);

    void update_map_texture(const unsigned char *bytes, const sf::Vector2u &dimensions,
                            const sf::Vector2u &position);
};
